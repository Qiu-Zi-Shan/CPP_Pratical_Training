#include "Mental_Trajectory.h"

const string PlayerManager::PLAYER_DATA_FILE = "C:\\Cpl\\CPP_Pratical_Training\\player_data.txt";
const string LeaderboardManager::CHALLENGE_LEADERBOARD_FILE= "C:\\Cpl\\CPP_Pratical_Training\\challenge_leaderboard.txt";;
const string LeaderboardManager::ADVENTURE_LEADERBOARD_FILE= "C:\\Cpl\\CPP_Pratical_Training\\adventure_leaderboard.txt";;
const string LeaderboardManager::LEVEL_TIME_FILE= "C:\\Cpl\\CPP_Pratical_Training\\level_time.txt";;


// Ship类的实现————————————————————
Ship::Ship(int startX, int startY){
    trajectory.emplace_back(startX, startY);
}

void Ship::move(int dx, int dy){
    trajectory.emplace_back(trajectory.back().x + dx, trajectory.back().y + dy);
}

// Player类的实现————————————————————
Player::Player() : score(0), totalGames(0), successGames(0), challengeScore(0), challengeGames(0), challengeSuccessGames(0){}
Player::Player(const string& name, const string& pwd) : username(name), password(pwd), 
                score(0), totalGames(0), successGames(0), 
                challengeScore(0), challengeGames(0), challengeSuccessGames(0){}

string Player::getUsername() const{ return username; }
bool Player::checkPassword(const string& pwd) const{ return password == pwd; }

// 娱乐模式分数相关方法
int Player::getScore() const{ return score; }
void Player::setScore(int newScore){ score = newScore; }
void Player::addScore(int points){ score += points; }
void Player::reduceScore(int points){ score -= points; }
void Player::addGame(bool success){ 
    totalGames++;
    if(success) successGames++;
}
int Player::getTotalGames() const{ return totalGames; }
int Player::getSuccessGames() const{ return successGames; }
double Player::getSuccessRate() const{
    return totalGames == 0 ? 0 : (double)(successGames) / totalGames * 100;
}

// 挑战模式分数相关方法
int Player::getChallengeScore() const { return challengeScore; }
void Player::setChallengeScore(int newScore) { challengeScore = newScore; }
void Player::addChallengeScore(int points) { challengeScore += points; }
void Player::reduceChallengeScore(int points) { challengeScore -= points; }
void Player::addChallengeGame(bool success) { 
    challengeGames++;
    if(success) challengeSuccessGames++;
}
int Player::getTotalChallengeGames() const { return challengeGames; }
int Player::getSuccessChallengeGames() const { return challengeSuccessGames; }
double Player::getChallengeSuccessRate() const {
    return challengeGames == 0 ? 0 : (double)(challengeSuccessGames) / challengeGames * 100;
}

// Player类等级方法实现
PlayerRank Player::getChallengeRank() const{ return calculateRank(challengeScore); }

string Player::getChallengeRankName() const{
    switch(getChallengeRank()){
        case PlayerRank::BRONZE: return "青铜";
        case PlayerRank::SILVER: return "白银";
        case PlayerRank::GOLD: return "黄金";
        case PlayerRank::PLATINUM: return "铂金";
        case PlayerRank::DIAMOND: return "钻石";
        case PlayerRank::MASTER: return "大师";
        default: return "未知";
    }
}

PlayerRank Player::calculateRank(int challengeScore){
    if(challengeScore < 50) return PlayerRank::BRONZE;
    else if(challengeScore < 150) return PlayerRank::SILVER;
    else if(challengeScore < 300) return PlayerRank::GOLD;
    else if(challengeScore < 500) return PlayerRank::PLATINUM;
    else if(challengeScore < 800) return PlayerRank::DIAMOND;
    else return PlayerRank::MASTER;
}

// GameRenderer类的实现————————————————————
void GameRenderer::printTrajectory(const vector<TrajectoryPoint>& trajectory, const string& name){
    cout << name << "轨迹：";
    for(const TrajectoryPoint& p : trajectory){
        cout << "(" << p.x << ", " << p.y << ") ";
    }
    cout << endl;
}

void GameRenderer::printCombinedGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& relativeB, const GameManager& gameManager){
    int gridSize = gameManager.gridSize; 
    char grid[gridSize][gridSize]; 
    
    // 初始化网格
    for(int y = 0; y < gridSize; y++){
        for(int x = 0; x < gridSize; x++){
            grid[y][x] = '.';
        }
    }
    
    // 标记A的航迹
    for(int i = 0; i < pathA.size(); i++){
        int x = pathA[i].x;
        int y = pathA[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            grid[y][x] = 'A';
        }
    }
    
    // 标记B的相对航迹
    for(int i = 0; i < relativeB.size(); i++){
        int x = relativeB[i].x;
        int y = relativeB[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            // 如果与A重叠，用'*'表示
            if(grid[y][x] == 'A'){
                grid[y][x] = '*';
            } 
            else{
                grid[y][x] = 'R'; // R表示相对轨迹
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
    cout << "图例: A=船A的航迹, R=船B的相对航迹, *=A和R重叠, .=空格" << endl;
}

void GameRenderer::printActualGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB, const GameManager& gameManager){
    int gridSize = gameManager.gridSize; 
    char grid[gridSize][gridSize]; 
    
    // 初始化网格
    for(int y = 0; y < gridSize; y++){
        for(int x = 0; x < gridSize; x++){
            grid[y][x] = '.';
        }
    }
    
    // 标记A的轨迹
    for(size_t i = 0; i < pathA.size(); i++){
        int x = pathA[i].x;
        int y = pathA[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            grid[y][x] = 'A';
        }
    }
    
    // 标记B的实际轨迹
    for(size_t i = 0; i < pathB.size(); i++){
        int x = pathB[i].x;
        int y = pathB[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            // 如果与A重叠，用'*'表示
            if(grid[y][x] == 'A'){
                grid[y][x] = '*';
            } 
            else{
                grid[y][x] = 'B'; // B表示B的实际轨迹
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
    cout << "图例: A=船A的实际航迹, B=船B的实际航迹, *=A和B重叠, .=空格" << endl;
}

// GameManager类的实现————————————————————
GameManager::GameManager()
     : shipA(0, 0), shipB(0, 0), difficulty(1), gameMode(0), gridSize(0), steps(0){
}

void GameManager::initializeGame(){
    // 随机选择游戏模式
    gameMode = rand() % 3;

    // 计算难度级别
    difficulty = calculateDifficulty();

    if(gameMode == 2){ difficulty += 2; } // 模式3难度上升2级

    //生成参照物A的实际航迹
    auto [aX, aY] = randomStart();
    shipA = Ship(aX, aY);
    generateShipTrajectory(shipA);

    //生成B的实际航迹
    auto [bX, bY] = randomStart();
    shipB = Ship(bX, bY);
    generateShipTrajectory(shipB);

    // 计算B相对于A的航迹
    relativeB = calculateRelativePath(shipA.trajectory, shipB.trajectory);  
}

pair<int, int> GameManager::randomStart(){
    return { rand() % gridSize, rand() % gridSize };
}

void GameManager::generateShipTrajectory(Ship& ship){
    // 定义可能的移动方向：上、右、下、左
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    int i = 0;  
    while(i < steps){    
        // 随机选择一个方向（0-3）
        int direction = rand() % 4;

        // 检查移动后是否会超出边界
        TrajectoryPoint last = ship.trajectory.back();
        int newX = last.x + dx[direction];
        int newY = last.y + dy[direction];
        if(newX < 0 || newX >= gridSize || newY < 0 || newY >= gridSize){ 
            continue;  // 重新选择方向
        }

        // 如果不是第一步，确保不会往回走
        if(i > 0){
            TrajectoryPoint prev = ship.trajectory[ship.trajectory.size() - 2];
            TrajectoryPoint curr = ship.trajectory.back();
                
            // 计算上一步的方向
            int lastDx = curr.x - prev.x;
            int lastDy = curr.y - prev.y;

            // 如果选择了相反的方向，重新选择
            if(dx[direction] == -lastDx && dy[direction] == -lastDy){ 
                continue;
            }
        }
                
        // 如果选择原地不动，重新选择
        if(dx[direction] == 0 && dy[direction] == 0){ 
            continue;
        }

        // 如果没有冲突，移动船只
        ship.move(dx[direction], dy[direction]);
        i++;  // 只有成功移动后才增加计数
    }
}

vector<TrajectoryPoint> GameManager::calculateRelativePath(const vector<TrajectoryPoint>& pathA,const vector<TrajectoryPoint>& pathB){
    vector<TrajectoryPoint> relative;

    // 第一个点是B的起始位置
    relative.emplace_back(pathB[0].x, pathB[0].y);
    
    // 计算每一步A的位移，并将其反向应用到B的位置上
     int X = pathB[0].x;
     int Y = pathB[0].y;
        
    for(size_t i = 1; i < pathB.size(); i++){
        // B的实际位移
        int bDx = pathB[i].x - pathB[i-1].x;
        int bDy = pathB[i].y - pathB[i-1].y;
            
        // A的实际位移
        int aDx = pathA[i].x - pathA[i-1].x;
        int aDy = pathA[i].y - pathA[i-1].y;
            
        // 从A的视角看，B的相对位移 = B的实际位移 - A的实际位移
        int relDx = bDx - aDx;
        int relDy = bDy - aDy;
            
        // 累加相对位移
        X += relDx;
        Y += relDy;

        relative.emplace_back(X, Y);
    }
    return relative;
}

int GameManager::calculateDifficulty(){
    int difficulty = 1;
    
    if(steps <= 6){ difficulty = 1; } 
    else if(steps <= 8){ difficulty = 2; } 
    else if(steps <= 10){ difficulty = 3; } 
    else if(steps <= 12){ difficulty = 4; }
    else{ difficulty = 5; }
    return difficulty;
}

int GameManager::calculatePoints(int difficulty, bool correct){
    if(correct){ return difficulty * 2; }
    else{ return difficulty; }
}

bool GameManager::playGameMode(Player& player, bool isChallenge){
    // 初始化游戏
    initializeGame();
    
    cout << "网格大小: " << gridSize << "x" << gridSize << endl;
    cout << "航迹步数: " << steps << endl;
    cout << "本次游戏难度级别: " << difficulty << "级 (1级最简单)" << endl;

    if(gameMode == 0){
        cout << "游戏模式：由B的相对航迹推测B的实际航迹" << endl;
    } 
    else if(gameMode == 1){
        cout << "游戏模式：由B的实际航迹推测B的相对航迹" << endl;
    }
    else{
        cout << "游戏模式：逐步推测B的航迹" << endl;
    }

    cout << "===== 脑力航迹 =====" << endl;
    
    bool correct = false;
    vector<TrajectoryPoint> playerAnswer;
    
    if(gameMode == 0){
        // 模式1：显示A的实际轨迹和B的相对轨迹
        GameRenderer::printTrajectory(shipA.trajectory, "船A的实际");
        GameRenderer::printTrajectory(relativeB, "船B的相对");
        
        cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
        GameRenderer::printCombinedGrid(shipA.trajectory, relativeB, *this);
        
        correct = processPlayerInputMode1(playerAnswer);
        displayResultMode1(correct, playerAnswer);
    } 
    else if(gameMode == 1){
        // 模式2：显示A的实际轨迹和B的实际轨迹
        GameRenderer::printTrajectory(shipA.trajectory, "船A的实际");
        GameRenderer::printTrajectory(shipB.trajectory, "船B的实际");
        
        cout << "\n参照物A和物体B的实际位置:" << endl;
        GameRenderer::printActualGrid(shipA.trajectory, shipB.trajectory, *this);
        
        correct = processPlayerInputMode2(playerAnswer);
        displayResultMode2(correct, playerAnswer);
    }
    else{
        // 模式3：逐步显示和验证
        vector<TrajectoryPoint> playerAnswer;
        correct = true;
        
        auto startTime = chrono::steady_clock::now();

        for(size_t step = 0; step < shipA.trajectory.size() && correct; ++step){
            // 随机决定这一步是猜实际航迹还是相对航迹
            bool isRelative = rand() % 2;
            
            // 显示A的当前轨迹
            vector<TrajectoryPoint> currentA(shipA.trajectory.begin(), shipA.trajectory.begin() + step + 1);
            GameRenderer::printTrajectory(currentA, "船A的实际");
            
            // 准备当前步骤的轨迹
            vector<TrajectoryPoint> currentB(shipB.trajectory.begin(), shipB.trajectory.begin() + step + 1);
            vector<TrajectoryPoint> currentRelativeB(relativeB.begin(), relativeB.begin() + step + 1);
            
            if(!isRelative){
                // 如果要猜B的实际航迹，显示完整的相对航迹，但实际航迹少显示一步
                GameRenderer::printTrajectory(currentRelativeB, "船B的相对");
                if(step > 0){
                    vector<TrajectoryPoint> prevB(shipB.trajectory.begin(), shipB.trajectory.begin() + step);
                    GameRenderer::printTrajectory(prevB, "船B的实际");
                }
                cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
                GameRenderer::printCombinedGrid(currentA, currentRelativeB, *this);
            } 
            else{
                // 如果要猜B的相对航迹，显示完整的实际航迹，但相对航迹少显示一步
                GameRenderer::printTrajectory(currentB, "船B的实际");
                if(step > 0){
                    vector<TrajectoryPoint> prevRelativeB(relativeB.begin(), relativeB.begin() + step);
                    GameRenderer::printTrajectory(prevRelativeB, "船B的相对");
                }
                cout << "\n参照物A的实际位置和物体B的实际航迹:" << endl;
                GameRenderer::printActualGrid(currentA, currentB, *this);
            }

            // 处理玩家输入
            correct = processPlayerInputMode3(playerAnswer, isRelative);

            // 显示当前步骤结果
            displayResultMode3(correct, playerAnswer, isRelative);
        }
        
        // 结束计时并显示总用时
        auto endTime = chrono::steady_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
        if(correct){
            cout << "\n恭喜完成挑战！总用时：" << duration.count() << " 秒。" << endl;
        } 
        else{
            cout << "\n挑战失败。本次用时：" << duration.count() << " 秒。" << endl;
        }
    }
    return correct;
}

bool GameManager::processPlayerInputMode1(vector<TrajectoryPoint>& playerAnswer){
    // 开始计时
    auto startTime = chrono::steady_clock::now();
    
    // 获取玩家答案
    cout << "\n请输入B的实际航迹（格式：x1,y1 x2,y2 ...）：" << endl;
    string input;
    getline(cin, input);

    // 检查作弊码
    if(input == "999"){
        playerAnswer = shipB.trajectory;
        return true;
    }

    // 结束计时并计算用时
    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    cout << "\n您花费了 " << duration.count() << " 秒完成答题。" << endl;

    // 解析输入
    stringstream ss(input);
    string point;
    while(ss >> point){
        size_t comma = point.find(',');
        int x = stoi(point.substr(0, comma));
        int y = stoi(point.substr(comma+1));
        playerAnswer.emplace_back(x, y);
    }

    // 验证答案
    bool correct = true;
    if(playerAnswer.size() != shipB.trajectory.size()){
        correct = false;
    } 
    else{
        for(size_t i = 0; i < shipB.trajectory.size(); i++){
            if(playerAnswer[i].x != shipB.trajectory[i].x || playerAnswer[i].y != shipB.trajectory[i].y){
                correct = false;
                break;
            }
        }
    }
    return correct;
}

bool GameManager::processPlayerInputMode2(vector<TrajectoryPoint>& playerAnswer){
    auto startTime = chrono::steady_clock::now();
        
    // 获取玩家答案
    cout << "\n请输入B的相对航迹（格式：x1,y1 x2,y2 ...）：" << endl;
    string input;
    getline(cin, input);

    // 结束计时并计算用时
    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    cout << "\n您花费了 " << duration.count() << " 秒完成答题。" << endl;

    // 检查作弊码
    if(input == "999"){
        playerAnswer = relativeB;
        return true;
    }

    // 原有的输入处理逻辑
    stringstream ss(input);
    string point;
    while(ss >> point){
        size_t comma = point.find(',');
        int x = stoi(point.substr(0, comma));
        int y = stoi(point.substr(comma+1));
        playerAnswer.emplace_back(x, y);
    }

    // 验证答案
    bool correct = true;
    if(playerAnswer.size() != relativeB.size()){
        correct = false;
    } 
    else{
        for(size_t i = 0; i < relativeB.size(); i++){
            if(playerAnswer[i].x != relativeB[i].x || playerAnswer[i].y != relativeB[i].y){
                correct = false;
                break;
            }
        }
    }
    return correct;
}

bool GameManager::processPlayerInputMode3(vector<TrajectoryPoint>& playerAnswer, bool isRelative){
    cout << "\n请输入B的" << (isRelative ? "相对" : "实际") << "航迹（格式：x,y）：" << endl;
    string input;
    getline(cin, input);

    if(input == "999"){
        if(isRelative){ 
            playerAnswer.push_back(relativeB[playerAnswer.size()]); 
            return true;
        }
        else{ 
            playerAnswer.push_back(shipB.trajectory[playerAnswer.size()]); 
            return true;
        }
    }

    size_t comma = input.find(',');
    int x = stoi(input.substr(0, comma));
    int y = stoi(input.substr(comma+1));
    playerAnswer.push_back(TrajectoryPoint(x, y));
    
    if(isRelative){
        return(playerAnswer.back().x == relativeB[playerAnswer.size()-1].x && 
        playerAnswer.back().y == relativeB[playerAnswer.size()-1].y);
    }
    else{
        return(playerAnswer.back().x == shipB.trajectory[playerAnswer.size()-1].x &&
        playerAnswer.back().y == shipB.trajectory[playerAnswer.size()-1].y);
    }
    return false;
}

void GameManager::displayResultMode1(bool correct, const vector<TrajectoryPoint>& playerAnswer){
    if(correct){
        cout << "正确！B的实际航迹：" << endl;
        GameRenderer::printTrajectory(shipB.trajectory, "B的实际");
        cout << "\n最终位置图示:" << endl;
        GameRenderer::printActualGrid(shipA.trajectory, shipB.trajectory, *this);
    } 
    else{
        cout << "错误！正确答案：" << endl;
        GameRenderer::printTrajectory(shipB.trajectory, "B的实际");
        cout << "\n正确的最终位置图示:" << endl;
        GameRenderer::printActualGrid(shipA.trajectory, shipB.trajectory, *this);
        
        cout << "\n您的答案图示:" << endl;
        // 显示玩家答案和A的实际轨迹
        GameRenderer::printActualGrid(shipA.trajectory, playerAnswer, *this);
    }
}

void GameManager::displayResultMode2(bool correct, const vector<TrajectoryPoint>& playerAnswer){
    if(correct){
        cout << "正确！B的相对航迹：" << endl;
        GameRenderer::printTrajectory(relativeB, "B的相对");
        cout << "\n最终位置图示:" << endl;
        // 显示A的实际轨迹和B的相对轨迹
        GameRenderer::printCombinedGrid(shipA.trajectory, relativeB, *this);
    } 
    else {
        cout << "错误！正确答案：" << endl;
        GameRenderer::printTrajectory(relativeB, "B的相对");
        cout << "\n正确的最终位置图示:" << endl;
        // 显示A的实际轨迹和B的相对轨迹
        GameRenderer::printCombinedGrid(shipA.trajectory, relativeB, *this);
        
        cout << "\n您的答案图示:" << endl;
        // 显示A的实际轨迹和玩家的答案
        GameRenderer::printCombinedGrid(shipA.trajectory, playerAnswer, *this);
    }
}

void GameManager::displayResultMode3(bool correct, const vector<TrajectoryPoint>& playerAnswer, bool isRelative){
    if(correct){
        cout << "正确！继续下一步。" << endl;
    } 
    else{
        cout << "错误！正确的" << (isRelative ? "相对" : "实际") << "位置应该是：";
        if(isRelative){
            cout << relativeB[playerAnswer.size()-1].x << "," << relativeB[playerAnswer.size()-1].y << endl;
        } 
        else{
            cout << shipB.trajectory[playerAnswer.size()-1].x << "," << shipB.trajectory[playerAnswer.size()-1].y << endl;
        }
    }
}

int GameManager::getDifficulty() const{ return difficulty; }

int GameManager::getGameMode() const{ return gameMode; }

int GameManager::generateGridSize(){ 
    gridSize = 5 + rand() % 6;  // 随机生成网格大小（范围：5-10）
    return gridSize;
}

int GameManager::generateSteps(){ 
    steps = gridSize + rand() % (gridSize / 2); // 根据网格大小，随机生成航迹步数
    return  steps;
}

void GameManager::setupForLevel(const Level& level){
    gridSize = level.gridSize;
    steps = level.steps;
    gameMode = level.gameMode;
}

void GameManager::setMaxDifficulty(int max){
    // 确保生成的难度不超过设定的上限
    difficulty = min(calculateDifficulty(), max);
}

const Ship& GameManager::getShipA() const{ return shipA; }

const Ship& GameManager::getShipB() const{ return shipB; }

const vector<TrajectoryPoint>& GameManager::getRelativeB() const{ return relativeB; }

// PlayerManager类实现————————————————————
bool PlayerManager::registerPlayer(const string& username, const string& password){
    // 检查用户名是否已存在
    ifstream inFile(PLAYER_DATA_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score;
            ss >> name >> pwd >> score;
            
            if(name == username){
                cout << "用户名已存在，请选择其他用户名。" << endl;
                inFile.close();
                return false;
            }
        }
        inFile.close();
    }
    
    // 添加新玩家到文件
    ofstream outFile(PLAYER_DATA_FILE, ios::app);
    if(outFile.is_open()){
        outFile << username << " " << password << " 0" << endl;
        outFile.close();
        cout << "注册成功！" << endl;
        return true;
    } 
    else{
        cout << "无法打开玩家数据文件，注册失败。" << endl;
        return false;
    }
}

bool PlayerManager::loginPlayer(const string& username, const string& password, Player& player){
    ifstream inFile(PLAYER_DATA_FILE);
    bool userFound = false;
    
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score, total, success;
            int challengeScore = 0, challengeTotal = 0, challengeSuccess = 0;
            ss >> name >> pwd >> score >> total >> success;
            
            // 尝试读取挑战模式的数据（可能不存在于旧数据中）
            if(ss >> challengeScore >> challengeTotal >> challengeSuccess){
                // 成功读取了挑战模式数据
            }

            if(name == username){
                userFound = true;
                if(pwd == password){
                    player = Player(username, password); 
                    player.setScore(score);
                    player.setChallengeScore(challengeScore);
                    
                    // 初始化娱乐模式数据
                    for(int i = 0; i < total; i++){
                        player.addGame(i < success);
                    }
                    
                    // 初始化挑战模式数据
                    for(int i = 0; i < challengeTotal; i++){
                        player.addChallengeGame(i < challengeSuccess);
                    }

                    cout << "登录成功！" << endl;
                    cout << "--------------------" << endl;
                    cout << "娱乐模式积分: " << score << endl;
                    cout << "游戏场次: " << total << "   成功: " << success 
                        << "   成功率: " << player.getSuccessRate() << "%" << endl << endl;
                        
                    cout << "挑战模式积分: " << challengeScore << endl;
                    cout << "当前等级: " << player.getChallengeRankName() << endl;
                    cout << "游戏场次: " << challengeTotal << "   成功: " << challengeSuccess
                        << "   成功率: " << player.getChallengeSuccessRate() << "%" << endl;
                    cout << "--------------------" << endl;
                    return true;
                } 
                else{ break; } // 用户名找到但密码错误，停止搜索
            }
        }
        inFile.close();
        
        if(userFound){ cout << "密码错误。" << endl; } 
        else { cout << "用户名不存在。" << endl; }
    } 
    else { cout << "无法打开玩家数据文件，登录失败。" << endl; }
    return false;
}

void PlayerManager::savePlayerData(const Player& player){
    vector<string> lines;
    ifstream inFile(PLAYER_DATA_FILE);
    string line;
    
    if(inFile.is_open()){
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            ss >> name >> pwd;

            if(name == player.getUsername()){
                // 更新该玩家的数据行
                line = name + " " + pwd + 
                       " " + to_string(player.getScore()) + 
                       " " + to_string(player.getTotalGames()) + 
                       " " + to_string(player.getSuccessGames()) +
                       " " + to_string(player.getChallengeScore()) +
                       " " + to_string(player.getTotalChallengeGames()) +
                       " " + to_string(player.getSuccessChallengeGames());
            }
            lines.push_back(line);
        }
        inFile.close();
        
        // 重新写入文件
        ofstream outFile(PLAYER_DATA_FILE);
        for(const auto& l : lines){ outFile << l << endl; }
        outFile.close();
    }
    else{ cout << "无法打开玩家数据文件，数据保存失败" << endl; }   
}

void PlayerManager::displayLeaderboard(){
    vector<tuple<string, int, double>> players;

    ifstream inFile(PLAYER_DATA_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score, total, success;
            int challengeScore, challengeTotal, challengeSuccess;
            ss >> name >> pwd >> score >> total >> success;

            // 尝试读取挑战模式数据，如果不存在则使用默认值
            if(!(ss >> challengeScore >> challengeTotal >> challengeSuccess)){
                challengeScore = 0;
                challengeTotal = 0;
                challengeSuccess = 0;
            }

            double rate = total > 0 ? (double)success / total * 100 : 0;
            players.push_back({name, score, rate});
        }
        inFile.close();
        
        // 按分数降序排序
        sort(players.begin(), players.end(), 
             [](const auto& a, const auto& b){
                 return get<1>(a) > get<1>(b);
             });
        
        cout << "\n===== 娱乐模式积分排行榜 =====" << endl;
        cout << "排名\t用户名\t积分\t成功率" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << get<0>(players[i]) << "\t" 
                 << get<1>(players[i]) << "\t"
                 << fixed << setprecision(1) << get<2>(players[i]) << "%" << endl;
        }
        
        if(players.empty()){ cout << "暂无玩家数据" << endl; }
    }
    else{ cout << "无法打开玩家数据文件，无法显示排行榜。" << endl; }
}

// ChallengeMode类实现————————————————
ChallengeMode::ChallengeMode(PlayerRank rank){
    maxDifficulty = getMaxDifficultyForRank(rank);
}

bool ChallengeMode::playChallenge(Player& player){
    gameManager.setMaxDifficulty(maxDifficulty);
    
    // 生成游戏网格和步数
    gameManager.generateGridSize();
    gameManager.generateSteps();
    
    cout << "===== 挑战积分模式 =====" << endl;
    cout << "当前等级: " << player.getChallengeRankName() << endl;
    cout << "当前挑战积分: " << player.getChallengeScore() << endl;
    cout << "本次挑战最高难度: " << maxDifficulty << endl;

    // 执行游戏
    bool result = gameManager.playGameMode(player, true); // 传入true表示这是挑战模式
    
    // 计算分数并更新
    int points = gameManager.calculatePoints(gameManager.getDifficulty(), result);
    
    if(result){
        cout << "恭喜！你获得了 " << points << " 点挑战积分！" << endl;
        player.addChallengeScore(points);
    } 
    else{
        if(player.getChallengeScore() > points){
            cout << "很遗憾，你失去了 " << points << " 点挑战积分。" << endl;
            player.reduceChallengeScore(points);
        } 
        else if(player.getChallengeScore() > 0){
            cout << "很遗憾，你失去了 " << player.getChallengeScore() << " 点挑战积分。" << endl;
            player.setChallengeScore(0);
        } 
        else{
            cout << "很遗憾，答错了。但你的挑战积分已经为0，不再扣分。" << endl;
        }
    }
    
    // 更新游戏记录
    player.addChallengeGame(result);
    
    cout << "当前挑战积分: " << player.getChallengeScore() << endl;
    cout << "当前等级: " << player.getChallengeRankName() << endl;
    
    // 保存到挑战模式排行榜
    LeaderboardManager::saveChallengeScore(player);
    
    return result;
}

int ChallengeMode::getMaxDifficultyForRank(PlayerRank rank){
    switch(rank){
        case PlayerRank::BRONZE: return 2;  
        case PlayerRank::SILVER: return 3;
        case PlayerRank::GOLD: return 4;   
        case PlayerRank::PLATINUM: return 5; 
        case PlayerRank::DIAMOND: return 6; 
        case PlayerRank::MASTER: return 7;   
        default: return 2;
    }
}

// AdventureMode类实现————————————————————
const vector<Level> AdventureMode::levels = {
    {1, 5, 6, 1, 150},   // 第1关：5x5网格，6步，模式1(推测相对轨迹)，150秒时限
    {2, 6, 7, 1, 145},   // 第2关：6x6网格，7步，模式1，145秒时限
    {3, 6, 8, 0, 145},   // 第3关：6x6网格，8步，模式0(推测实际轨迹)，145秒时限
    {4, 7, 8, 0, 140},   // 第4关：7x7网格，8步，模式0，140秒时限
    {5, 7, 9, 2, 130},  // 第5关：7x7网格，9步，模式2(交替推测)，130秒时限
    {6, 8, 10, 2, 125}, // 第6关：8x8网格，10步，模式2，125秒时限
    {7, 9, 11, 2, 115}, // 第7关：9x9网格，11步，模式2，115秒时限
    {8, 10, 12, 2, 100} // 第8关：10x10网格，12步，模式2，100秒时限
};

AdventureMode::AdventureMode() : currentLevel(1){}

bool AdventureMode::playLevel(Player& player, int levelNumber){
    if(levelNumber <= 0 || levelNumber > levels.size()){
        cout << "关卡不存在！" << endl;
        return false;
    }
    
    // 获取玩家已解锁的最高关卡
    int highestUnlocked = getHighestUnlockedLevel(player.getUsername());
    
    // 检查玩家是否有权限挑战该关卡
    if(levelNumber > highestUnlocked){
        cout << "请先通过第" << highestUnlocked << "关才能挑战第" << levelNumber << "关！" << endl;
        return false;
    }

    currentLevel = levelNumber;
    const Level& level = levels[levelNumber - 1];
    
    cout << "===== 挑战第" << level.levelNumber << "关 =====" << endl;
    cout << "网格大小: " << level.gridSize << "x" << level.gridSize << endl;
    cout << "步数: " << level.steps << endl;
    cout << "时间限制: " << level.timeLimit << "秒" << endl;
    
    // 设置游戏管理器
    gameManager.setupForLevel(level);
    
    // 开始计时
    auto startTime = chrono::steady_clock::now();
    
    // 执行游戏
    bool result = gameManager.playGameMode(player, false);
    
    // 结束计时
    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    int timeSeconds = duration.count();
    
    cout << "用时: " << timeSeconds << "秒" << endl;
    
    // 检查是否超时
    if(timeSeconds > level.timeLimit){
        cout << "超出时间限制！关卡失败。" << endl;
        result = false;
    }
    
    if(result){
        // 保存关卡完成记录和用时
        LeaderboardManager::saveAdventureProgress(player.getUsername(), levelNumber);
        LeaderboardManager::saveLevelTime(player.getUsername(), levelNumber, timeSeconds);
        
        cout << "恭喜通过第" << levelNumber << "关！" << endl;
        
        // 显示排行榜选项
        displayLeaderboardOptions(levelNumber);
        
        // 如果不是最后一关，并且刚刚解锁了下一关
        if(levelNumber < levels.size() && levelNumber == highestUnlocked){
            cout << "你已解锁第" << (levelNumber + 1) << "关！" << endl;
            cout << "是否挑战下一关? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore();
            
            if(choice == 'y'){
                return playLevel(player, levelNumber + 1);
            }
        }
    } 
    else{ cout << "关卡失败。请再接再厉！" << endl; }
    
    return result;
}

int AdventureMode::getCurrentLevel() const{ return currentLevel; }

int AdventureMode::getLevelCount(){ return levels.size(); }

int AdventureMode::getHighestUnlockedLevel(const string& username){
    int highestLevel = 0;
    
    ifstream inFile("C:\\Cpl\\CPP_Pratical_Training\\adventure_leaderboard.txt");
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int level;
            ss >> name >> level;
            
            if(name == username){
                highestLevel = level;
                break;
            }
        }
        inFile.close();
    }
    // 如果找不到记录，默认第一关可用
    return max(1, highestLevel);
}

void AdventureMode::displayLeaderboardOptions(int levelNumber){
    bool viewLeaderboard = true;
    
    while(viewLeaderboard){
        cout << "\n查看排行榜：" << endl;
        cout << "1. 查看闯关总排行榜" << endl;
        cout << "2. 查看本关用时排行榜" << endl;
        cout << "3. 继续游戏" << endl;
        cout << "请选择: ";
        
        int choice;
        cin >> choice;
        cin.ignore();
        
        switch(choice){
            case 1:
                LeaderboardManager::displayAdventureLeaderboard();
                break;
            case 2:
                LeaderboardManager::displayLevelTimeLeaderboard(levelNumber);
                break;
            case 3:
                viewLeaderboard = false;
                break;
            default:
                cout << "无效选择，请重试。" << endl;
        }
    }
}

// LeaderboardManager类实现————————————————————
void LeaderboardManager::saveChallengeScore(const Player& player){
    vector<tuple<string, int, string, double>> players;
    bool playerFound = false;
    
    // 读取现有排行榜
    ifstream inFile(CHALLENGE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, rank;
            int score;
            double rate;
            ss >> name >> score >> rank >> rate;
            
            if(name == player.getUsername()){ continue; }
            players.push_back({name, score, rank, rate});
        }
        inFile.close();
    }
    
    // 添加当前玩家数据
    players.push_back({player.getUsername(), 
                      player.getChallengeScore(), 
                      player.getChallengeRankName(), 
                      player.getChallengeSuccessRate()});
    
    // 按分数降序排序，同分按成功率降序
    sort(players.begin(), players.end(), 
         [](const auto& a, const auto& b){
             if(get<1>(a) != get<1>(b)){
                 return get<1>(a) > get<1>(b);
             }
             return get<3>(a) > get<3>(b);  // 比较成功率
         });
    
    // 写回文件
    ofstream outFile(CHALLENGE_LEADERBOARD_FILE);
    if(outFile.is_open()){
        for(const auto& p : players){
            outFile << get<0>(p) << " " << get<1>(p) << " " 
                   << get<2>(p) << " " << get<3>(p) << endl;
        }
        outFile.close();
    } 
    else{
        cout << "无法打开排行榜文件，保存失败" << endl;
    }
}

void LeaderboardManager::saveAdventureProgress(const string& username, int level){
    vector<pair<string, int>> players;
    
    // 读取现有数据
    ifstream inFile(ADVENTURE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int maxLevel;
            ss >> name >> maxLevel;
            
            // 如果找到当前玩家且新关卡更高，更新
            if(name == username){ maxLevel = max(maxLevel, level); }
            players.push_back({name, maxLevel});
        }
        inFile.close();
    }
    
    // 如果没找到当前玩家，添加
    auto it = find_if(players.begin(), players.end(), 
                     [&username](const auto& p){ return p.first == username; });
    if(it == players.end()){
        players.push_back({username, level});
    }
    
    // 按关卡降序排序
    sort(players.begin(), players.end(), 
         [](const auto& a, const auto& b){ return a.second > b.second; });
    
    // 写回文件
    ofstream outFile(ADVENTURE_LEADERBOARD_FILE);
    if(outFile.is_open()){
        for(const auto& p : players){
            outFile << p.first << " " << p.second << endl;
        }
        outFile.close();
    }
}

void LeaderboardManager::saveLevelTime(const string& username, int level, int timeSeconds){
    vector<tuple<string, int, int>> records; // 用户名、关卡、用时
    
    // 读取现有记录
    ifstream inFile(LEVEL_TIME_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int lvl, time;
            ss >> name >> lvl >> time;
            
            // 如果找到相同用户和关卡的记录，跳过
            if(name == username && lvl == level){
                continue;
            }
            
            records.push_back({name, lvl, time});
        }
        inFile.close();
    }
    
    // 添加新记录
    records.push_back({username, level, timeSeconds});
    
    // 写回文件
    ofstream outFile(LEVEL_TIME_FILE);
    if(outFile.is_open()){
        for(const auto& r : records){
            outFile << get<0>(r) << " " << get<1>(r) << " " << get<2>(r) << endl;
        }
        outFile.close();
    }
}

void LeaderboardManager::displayChallengeLeaderboard(){
    vector<tuple<string, int, string, double>> players; 
    
    ifstream inFile(CHALLENGE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, rank;
            int score;
            double rate;
            ss >> name >> score >> rank >> rate;
            
            players.push_back({name, score, rank, rate});
        }
        inFile.close();
        
        cout << "\n===== 挑战积分排行榜 =====" << endl;
        cout << "排名\t用户名\t积分\t等级\t成功率" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << get<0>(players[i]) << "\t" 
                 << get<1>(players[i]) << "\t"
                 << get<2>(players[i]) << "\t"
                 << fixed << setprecision(1) << get<3>(players[i]) << "%" << endl;
        }
        
        if(players.empty()){
            cout << "暂无玩家数据" << endl;
        }
    } 
    else{
        cout << "无法打开排行榜文件。" << endl;
    }
}

void LeaderboardManager::displayAdventureLeaderboard(){
    vector<pair<string, int>> players;
    
    ifstream inFile(ADVENTURE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int level;
            ss >> name >> level;
            players.push_back({name, level});
        }
        inFile.close();
        
        // 按关卡降序排序
        sort(players.begin(), players.end(), 
             [](const auto& a, const auto& b){ return a.second > b.second; });
        
        cout << "\n===== 闯关模式排行榜 =====" << endl;
        cout << "排名\t用户名\t已通过关卡" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << players[i].first << "\t" 
                 << players[i].second << endl;
        }
        
        if(players.empty()){
            cout << "暂无玩家数据" << endl;
        }
    }
    else{
        cout << "无法打开排行榜文件。" << endl;
    }
}

void LeaderboardManager::displayLevelTimeLeaderboard(int level){
    vector<pair<string, int>> timesForLevel; // 用户名、用时
    
    ifstream inFile(LEVEL_TIME_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int lvl, time;
            ss >> name >> lvl >> time;
            
            if(lvl == level){
                timesForLevel.push_back({name, time});
            }
        }
        inFile.close();
        
        // 按用时升序排序
        sort(timesForLevel.begin(), timesForLevel.end(), 
             [](const auto& a, const auto& b){ return a.second < b.second; });
        
        cout << "\n===== 第" << level << "关用时排行榜 =====" << endl;
        cout << "排名\t用户名\t用时(秒)" << endl;
        
        for(size_t i = 0; i < timesForLevel.size(); ++i){
            cout << i + 1 << "\t" 
                 << timesForLevel[i].first << "\t" 
                 << timesForLevel[i].second << endl;
        }
        
        if(timesForLevel.empty()){
            cout << "暂无完成此关卡的玩家" << endl;
        }
    } 
    else{ cout << "无法打开记录文件。" << endl; }
}
