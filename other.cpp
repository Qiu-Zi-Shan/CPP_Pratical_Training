#include "Mental_Trajectory.h"

Ship::Ship(int startX, int startY){
    trajectory.emplace_back(startX, startY);
}
void Ship::move(int dx, int dy){
    trajectory.emplace_back(trajectory.back().x + dx, trajectory.back().y + dy);
}

TimeEngine::TimeEngine() : startTime(), endTime(), pausedDuration(chrono::seconds(0)), isPaused(false){}

void TimeEngine::start(){
    startTime = chrono::steady_clock::now();
}
void TimeEngine::end(){
    endTime = chrono::steady_clock::now();
}
void TimeEngine::pause(){
    pauseStartTime = chrono::steady_clock::now();
}
void TimeEngine::resume(){
    auto pauseEndTime = chrono::steady_clock::now();
    pausedDuration += (pauseEndTime - pauseStartTime);
}
void TimeEngine::reducePausedDuration(chrono::seconds penaltyTime){
    pausedDuration -= penaltyTime;
}
void TimeEngine::displayTimeCost(){
    cout << "\n==== 游戏结束 ====" << endl;
    cout << "游戏用时: " << getTimeCost().count() << " 秒" << endl;
}

chrono::seconds TimeEngine::getTimeCost(){ 
    return chrono::duration_cast<chrono::seconds>(getEndTime() - getStartTime() - getPausedDuration()); 
}

void GameRenderer::printTrajectory(const vector<TrajectoryPoint>& trajectory, const string& name){
    cout << name << "轨迹：";
    for(const TrajectoryPoint& p : trajectory){
        cout << "(" << p.x << ", " << p.y << ") ";
    }
    cout << endl;
}
void GameRenderer::printGrid(const vector<TrajectoryPoint>& actualshipA, const vector<TrajectoryPoint>& shipB, const AbstractInitializer* initializer,bool isRelative){
    int gridSize = initializer->getGridSize(); 
    char grid[gridSize][gridSize]; 
    
    // 初始化网格
    for(int y = 0; y < gridSize; y++){
        for(int x = 0; x < gridSize; x++){
            grid[y][x] = '.';
        }
    }
    
    // 标记A的航迹
    for(int i = 0; i < actualshipA.size(); i++){
        int x = actualshipA[i].x;
        int y = actualshipA[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            grid[y][x] = 'A';
        }
    }
    
    // 标记B航迹
    for(int i = 0; i < shipB.size(); i++){
        int x = shipB[i].x;
        int y = shipB[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            // 如果与A重叠，用'*'表示
            if(grid[y][x] == 'A'){
                grid[y][x] = '*';
            } 
            else{
                if(isRelative){ grid[y][x] = 'R'; }
                else { grid[y][x] = 'B';}
            }
        }
    }
    
    // 打印网格
    cout << "  ";
    for(int x = 0; x < gridSize; x++){
        cout << x << " ";
    }
    cout << endl;
    
    for(int y = 0; y < gridSize; y++){
        cout << y << " ";
        for(int x = 0; x < gridSize; x++){
            cout << grid[y][x] << " ";
        }
        cout << endl;
    }
    cout << "图例: A=船A的航迹, B=船B的航迹, R=船B的相对航迹, *=A和B或R重叠, .=空格" << endl;
}

void ChallengeMode::playChallengeMode(Player& player){
    // 初始化游戏
    AbstractInitializer* initializer = new ChallengeInitializer(player);
    // 显示游戏信息
    cout << "\n===== 挑战模式 =====" << endl;
    cout << "游戏难度: " << initializer->getDifficulty() << endl;
    // 执行游戏
    bool result = initializer->getBaseGameModeEngine().startBaseGameMode(initializer);

    // 显示玩家游戏结果
    if(result){
        cout << "您获得了 " << 2 * initializer->getPoints() << " 分！" << endl;
        player.addChallengeScore(2 * initializer->getPoints());
    }
    else{
        if(player.getChallengeScore() > initializer->getPoints()){
            cout << "您失去了 " << initializer->getPoints() << " 分。" << endl;
            player.reduceChallengeScore(initializer->getPoints());
        } 
        else if(player.getChallengeScore() > 0){
            cout << "您失去了 " << player.getChallengeScore() << " 分。" << endl;
            player.setChallengeScore(0);
        } 
        else{
            cout << "分数已经为0，不再扣分。" << endl;
        }
    }    
    cout << "\n==== 当前挑战模式积分: " << player.getChallengeScore() << " ====" << endl;

    // 更新玩家数据
    player.addChallengeGame(result);
    PlayerManager::updatePlayerData(player);
    LeaderboardManager::updateChallengeLeaderboard(player);

    delete initializer;
}

void StageMode::displayAvailableStage(Player& player){
    cout << "当前可闯关卡: " << endl;
    for(int i = 1; i <= player.getCurrentStage() + 1; i++){
        cout << i << " " << endl;
    }
}
void StageMode::playStageMode(Player& player){
    cout << "\n===== 闯关模式 =====" << endl;
    int stageChoice;
    bool validChoice = true;
    while(validChoice){
        displayAvailableStage(player);
        cout << "请选择闯关模式: ";
        cin >> stageChoice;
        cin.ignore();
    
        if(stageChoice > player.getCurrentStage() + 1){
            cout << "当前关卡尚未解锁，请先闯过前面的关卡。" << endl;
        }
        else{ validChoice = false; }
    }
    // 初始化游戏
    AbstractInitializer* initializer = new StageInitializer(player, stageChoice);
    
    // 设置为闯关模式（启用时间限制）
    initializer->getBaseGameModeEngine().getCurrentBaseMode()->setIsStageMode(true);
    
    cout << "【闯关模式】每个关卡限时150秒，超时将视为挑战失败！" << endl;
    cout << "【提示功能】输入'h'或'H'获取提示，每关限用一次，使用后减少15秒时间！" << endl;
    
    // 执行游戏
    bool result = initializer->getBaseGameModeEngine().startBaseGameMode(initializer);
    // 获取闯关用时
    int timeCost = initializer->getBaseGameModeEngine().getTimeCost().count();

    // 显示玩家游戏结果
    if(result){ 
        if(timeCost <= 150){
            cout << "闯关成功！" << endl;
            cout << "本次用时: " << timeCost << " 秒" << endl;
            
            // 如果通过新关卡，更新当前关卡进度
            if(stageChoice > player.getCurrentStage()){
                player.setCurrentStage(stageChoice);
            }
            
            // 更新用时记录
            player.setStageTime(stageChoice, timeCost);
            
            // 更新排行榜
            LeaderboardManager::updateStageTimeLeaderboard(player, stageChoice, timeCost);
        } 
        else{
            cout << "时间超过150秒限制，闯关失败！" << endl;
        }
    }
    else{ 
        cout << "闯关失败!" << endl; 
        if(timeCost > 150){
            cout << "超过时间限制(150秒)，请尝试更快完成。" << endl;
        }
    }

    // 更新玩家数据
    PlayerManager::updatePlayerData(player);
    LeaderboardManager::updateStageLeaderboard(player);

    delete initializer;
}

void FunMode::playFunMode(Player& player){
    cout << "\n===== 娱乐模式 =====" << endl;
    cout << "请选择游戏难度(1-3): ";
    int gameDifficulty;
    cin >> gameDifficulty;
    cout << "\n加载地图中..." << endl;
    FunModeEngine engine(player, gameDifficulty);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    bool result = engine.startFunMode();

    if(result){
        cout << "恭喜! 你获得了" << gameDifficulty << "分!" << endl;
        player.addFunScore(gameDifficulty);
    }
    else{
        cout << "很遗憾，你输了。" << endl;
    }
     // 更新玩家数据
    player.addFunGame(result);
    PlayerManager::updatePlayerData(player);
    LeaderboardManager::updateFunLeaderboard(player);
}

void GameRunner::runGame(){
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0)));

    // 玩家登录/注册
    Player currentPlayer;
    bool continueGame = true;
    
    while(continueGame){
        bool loggedIn = false;
        bool playAgain = true;
        while(!loggedIn){
            cout << "===== 欢迎来到脑力航迹游戏 =====" << endl;
            cout << "1. 登录" << endl;
            cout << "2. 注册" << endl;
            cout << "3. 退出游戏" << endl;
            cout << "请选择操作: ";

            int choice;
            cin >> choice;
            cin.ignore(); // 清除输入缓冲区中的换行符

            string username, password;

            switch(choice){
                case 1: // 登录
                    cout << "\n==== 登录 ====" << endl;
                    cout << "请输入用户名: ";
                    getline(cin, username);
                    cout << "请输入密码: ";
                    getline(cin, password);
                    loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                    break;
                case 2: // 注册
                    cout << "\n==== 注册 ====" << endl;
                    cout << "请输入用户名: ";
                    getline(cin, username);
                    cout << "请输入密码: ";
                    getline(cin, password);

                    if(PlayerManager::registerPlayer(username, password)){
                        cout << "是否立即登录? (y/n): ";
                        char loginChoice;
                        cin >> loginChoice;
                        cin.ignore(); 
                        
                        if(loginChoice == 'y' || loginChoice == 'Y'){
                            loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                        }
                    }
                    break;
                case 3: // 退出
                    cout << "感谢游玩，再见！" << endl;
                    playAgain = false;
                    continueGame = false;
                    break;
                default:
                    cout << "无效选择，请重试。" << endl;
            }
        }

        // 游戏主循环
        while(playAgain){
            // 主菜单
            cout << "\n===== 主菜单 =====" << endl;
            cout << "1. 开始游戏" << endl;
            cout << "2. 查看排行榜" << endl;
            cout << "3. 返回登录注册页面" << endl;
            cout << "4. 退出游戏" << endl;
            cout << "请选择: ";
            
            int choice;
            cin >> choice;
            cin.ignore(); 

            switch(choice){
                case 1:{
                    cout << "\n===== 游戏操作说明 =====" << endl;
                    cout << "【暂停操作】在游戏过程中，您可以按 'p/P' 键随时暂停/继续游戏，暂停期间计时会停止" << endl;
                    cout << "【提示操作】在闯关模式中，您可以按 'h/H' 键获取提示，每关限用一次，使用后减少15秒时间（仅闯关模式中有效）" << endl;
                    bool invalidChoice = true;
                    while(invalidChoice){ 
                        // 获取玩家选择的游戏模式
                        cout << "\n===== 游戏模式选择 =====" << endl;
                        cout << "1. 挑战模式 (积分: " << currentPlayer.getChallengeScore() 
                            << ", 等级: " << currentPlayer.getChallengeRankName() << ")" << endl;
                        cout << "2. 闯关模式" << endl;
                        cout << "3. 娱乐模式 (积分: " << currentPlayer.getFunScore() << ")" << endl;
                        cout << "4. 返回主菜单" << endl;
                        cout << "请选择: ";

                        int modeChoice;
                        cin >> modeChoice;
                        cin.ignore(); 
                        
                        switch(modeChoice){
                            case 1: // 挑战模式
                                invalidChoice = false;
                                ChallengeMode::playChallengeMode(currentPlayer);
                                break;
                            case 2: // 闯关模式
                                invalidChoice = false;
                                StageMode::playStageMode(currentPlayer);
                                break;
                            case 3: // 娱乐模式
                                invalidChoice = false;
                                FunMode::playFunMode(currentPlayer);
                                break;
                            case 4: // 返回主菜单
                                invalidChoice = false;
                                break;
                            default:
                                cout << "无效选择，请重新选择。" << endl;
                                break;
                        }
                    }
                    break;
                }
                case 2:{
                    bool invalidChoice = true;
                    while(invalidChoice){
                        cout << "\n===== 选择要查看的排行榜 =====" << endl;
                        cout << "1. 挑战模式排行榜" << endl;
                        cout << "2. 闯关模式排行榜" << endl;
                        cout << "3. 娱乐模式排行榜" << endl;
                        cout << "请选择: ";
                        
                        int leaderboardChoice;
                        cin >> leaderboardChoice;
                        cin.ignore(); 
                        
                        switch(leaderboardChoice){
                            case 1:
                                invalidChoice = false;
                                LeaderboardManager::displayChallengeLeaderboard();
                                break;
                            case 2:
                                invalidChoice = false;
                                // 闯关模式排行榜子菜单
                                int stageLeaderboardChoice;
                                cout << "\n===== 选择要查看的闯关排行榜类型 =====" << endl;
                                cout << "1. 闯关数排行榜" << endl;
                                cout << "2. 闯关用时排行榜" << endl;
                                cout << "请选择: ";
                                cin >> stageLeaderboardChoice;
                                cin.ignore();
                                
                                if(stageLeaderboardChoice == 1){
                                    LeaderboardManager::displayStageLeaderboard();
                                } 
                                else if(stageLeaderboardChoice == 2){
                                    LeaderboardManager::displayStageTimeLeaderboardMenu();
                                } 
                                else { cout << "无效选择" << endl; }
                                break;
                            case 3:
                                invalidChoice = false;
                                LeaderboardManager::displayFunLeaderboard();
                                break;
                            default:
                                cout << "无效选择，请重新选择。" << endl;
                                break;
                        }
                    }
                    break;
                }
                case 3: // 返回游戏登录注册页面
                    loggedIn = false;
                    playAgain = false;
                    break;
                case 4: // 退出游戏
                    cout << "感谢使用，再见！" << endl;
                    playAgain = false;
                    continueGame = false;
                    break;
                default:
                    cout << "无效选择，请重新选择。" << endl;
            }
        }
    }
}
