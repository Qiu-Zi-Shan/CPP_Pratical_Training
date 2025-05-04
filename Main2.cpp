#include "Mental_Trajectory.h"

int main(){
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0))); 

    // 玩家登录/注册
    Player currentPlayer;
    bool loggedIn = false;
    
    cout << "===== 欢迎来到脑力航迹游戏 =====" << endl;
    
    while(!loggedIn){
        cout << "1. 登录" << endl;
        cout << "2. 注册" << endl;
        cout << "3. 查看排行榜" << endl;
        cout << "4. 退出" << endl;
        cout << "请选择操作: ";
        
        int choice;
        cin >> choice;
        cin.ignore(); // 清除输入缓冲区
        
        string username, password;
        
        switch(choice){
            case 1: // 登录
                cout << "请输入用户名: ";
                getline(cin, username);
                cout << "请输入密码: ";
                getline(cin, password);
                
                loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                break;
                
            case 2: // 注册
                cout << "请输入用户名: ";
                getline(cin, username);
                cout << "请输入密码: ";
                getline(cin, password);
                
                if(PlayerManager::registerPlayer(username, password)){
                    cout << "是否立即登录? (y/n): ";
                    char loginChoice;
                    cin >> loginChoice;
                    cin.ignore();
                    
                    if(loginChoice == 'y'){
                        loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                    }
                }
                break;
                
            case 3: // 查看排行榜
                int leaderboardChoice;
                cout << "选择要查看的排行榜：" << endl;
                cout << "1. 娱乐模式排行榜" << endl;
                cout << "2. 挑战积分模式排行榜" << endl;
                cout << "3. 闯关模式排行榜" << endl;
                cout << "4. 关卡用时排行榜" << endl;
                cout << "请选择: ";
                cin >> leaderboardChoice;
                cin.ignore();
                switch(leaderboardChoice){
                    case 1:
                        PlayerManager::displayLeaderboard();
                        break;
                    case 2:
                        LeaderboardManager::displayChallengeLeaderboard();
                        break;
                    case 3:
                        LeaderboardManager::displayAdventureLeaderboard();
                        break;
                    case 4:
                        int level;
                        cout << "请输入要查看的关卡号(1-" << AdventureMode::getLevelCount() << "): ";
                        cin >> level;
                        cin.ignore();
                        LeaderboardManager::displayLevelTimeLeaderboard(level);
                        break;
                    default:
                        cout << "无效选择。" << endl;
                }
                break;
                
            case 4: // 退出
                cout << "感谢使用，再见！" << endl;
                return 0;
                
            default:
                cout << "无效选择，请重试。" << endl;
        }
    }
    // 游戏主循环
    bool playAgain = true;
    
    while(playAgain){
        cout << "\n===== 游戏模式选择 =====" << endl;
        cout << "1. 娱乐模式 (积分: " << currentPlayer.getScore() << ")" << endl;
        cout << "2. 挑战积分模式 (积分: " << currentPlayer.getChallengeScore() 
            << ", 等级: " << currentPlayer.getChallengeRankName() << ")" << endl;
        cout << "3. 闯关模式" << endl;
        cout << "4. 返回主菜单" << endl;
        cout << "请选择: ";
        
        int modeChoice;
        cin >> modeChoice;
        cin.ignore();
        
        bool gameResult = false;
        
        switch(modeChoice){
            case 1: {
                // 娱乐模式
                GameManager gameManager;
                gameManager.generateGridSize();
                gameManager.generateSteps();
                // 执行游戏
                bool result = gameManager.playGameMode(currentPlayer, false); // 传入false表示这是娱乐模式
                
                // 计算分数并更新
                int points = gameManager.calculatePoints(gameManager.getDifficulty(), result);
                
                if(result){
                    cout << "恭喜！你获得了 " << points << " 分！" << endl;
                    currentPlayer.addScore(points);
                }
                else{
                    if(currentPlayer.getScore() > points){
                        cout << "很遗憾，你失去了 " << points << " 分。" << endl;
                        currentPlayer.reduceScore(points);
                    } 
                    else if(currentPlayer.getScore() > 0){
                        cout << "很遗憾，你失去了 " << currentPlayer.getScore() << " 分。" << endl;
                        currentPlayer.setScore(0);
                    } 
                    else{
                        cout << "很遗憾，答错了。但你的分数已经为0，不再扣分。" << endl;
                    }
                }
                
                // 更新游戏记录
                currentPlayer.addGame(result);
                
                cout << "当前娱乐模式积分: " << currentPlayer.getScore() << endl;
                
                // 保存玩家数据
                PlayerManager::savePlayerData(currentPlayer);
                PlayerManager::displayLeaderboard();
                
                gameResult = result;
                break;
            }
            
            case 2: {
                // 挑战积分模式
                PlayerRank rank = currentPlayer.getChallengeRank();
                
                ChallengeMode challengeMode(rank);
                gameResult = challengeMode.playChallenge(currentPlayer);
                
                // 保存玩家数据
                PlayerManager::savePlayerData(currentPlayer);
                
                // 显示排行榜
                LeaderboardManager::displayChallengeLeaderboard();
                break;
            }
            
            case 3: {
                // 闯关模式
                AdventureMode adventureMode;
                
                // 获取玩家已解锁的最高关卡
                int highestUnlocked = AdventureMode::getHighestUnlockedLevel(currentPlayer.getUsername());

                cout << "\n===== 闯关模式 =====" << endl;
                cout << "你当前已解锁到第" << highestUnlocked << "关（共" << AdventureMode::getLevelCount() << "关）" << endl;
                
                // 显示可选关卡
                cout << "可挑战关卡: ";
                for(int i = 1; i <= highestUnlocked; i++){
                    cout << i << " ";
                }
                cout << endl;
                
                cout << "请选择要挑战的关卡: ";
                int levelChoice;
                cin >> levelChoice;
                cin.ignore();
                
                // 检查关卡选择是否有效
                if(levelChoice < 1 || levelChoice > highestUnlocked){
                    cout << "无效的关卡选择！" << endl;
                } 
                else{
                    gameResult = adventureMode.playLevel(currentPlayer, levelChoice);
                    
                    // 保存玩家数据
                    PlayerManager::savePlayerData(currentPlayer);
                }
                break;
            }
            
            case 4:
                playAgain = false;
                continue;
                
            default:
                cout << "无效选择，请重试。" << endl;
                continue;
        }
        
        // 询问是否继续游戏
        if(playAgain){
            cout << "是否继续游戏? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore();
            
            playAgain = (choice == 'y');
        }
    }
    
    cout << "感谢游玩，再见！" << endl;
    return 0;
}