#include "Ship.h"
#include<iostream>
#include<vector>
#include<cstdlib>
#include<sstream>
#include<fstream>
#include<algorithm>
#include<chrono> 
#include<iomanip>
using namespace std;

// 初始化位置
Ship::Ship(int startX, int startY) {
    trajectory.emplace_back(startX, startY);
}

// 添加移动轨迹
void Ship::move(int dx, int dy) {
    TrajectoryPoint last = trajectory.back();
    trajectory.emplace_back(last.x + dx, last.y + dy);
}

// 生成随机起始位置
pair<int, int> randomStart() {
    return {rand() % GRID_SIZE, rand() % GRID_SIZE};
}

// 打印轨迹
void printTrajectory(const vector<TrajectoryPoint>& path, const string& name) {
    cout << name << "航迹：";
     for (const TrajectoryPoint& p : path) {
        cout << "(" << p.x << "," << p.y << ") ";
    }
    cout << endl;
}

// 计算相对轨迹（以B的起始位置为起点，但表示相对于A的运动）
vector<TrajectoryPoint> calculateRelativePath(const vector<TrajectoryPoint>& base,const vector<TrajectoryPoint>& target) {
    vector<TrajectoryPoint> relative;

    // 第一个点是B的起始位置
    relative.emplace_back(target[0].x, target[0].y);
    
    // 计算每一步A的位移，并将其反向应用到B的位置上
     int currentX = target[0].x;
     int currentY = target[0].y;
        
    for(size_t i = 1; i < target.size(); i++){
        // B的实际位移
        int bDx = target[i].x - target[i-1].x;
        int bDy = target[i].y - target[i-1].y;
            
        // A的实际位移
        int aDx = base[i].x - base[i-1].x;
        int aDy = base[i].y - base[i-1].y;
            
        // 从A的视角看，B的相对位移 = B的实际位移 - A的实际位移
        int relDx = bDx - aDx;
        int relDy = bDy - aDy;
            
        // 累加相对位移
        currentX += relDx;
        currentY += relDy;
            
        relative.emplace_back(currentX, currentY);
    }
    return relative;
}

// 打印网格世界（包含A的实际轨迹和B的相对轨迹）
void printCombinedGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& relativePathB) {
    char grid[GRID_SIZE][GRID_SIZE];
    
    // 初始化网格
    for(int y = 0; y < GRID_SIZE; y++){
        for(int x = 0; x < GRID_SIZE; x++){
            grid[y][x] = '.';
        }
    }
    
    // 标记A的轨迹
    for(int i = 0; i < pathA.size(); i++){
        int x = pathA[i].x;
        int y = pathA[i].y;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE){
            grid[y][x] = 'A';
        }
    }
    
    // 标记B的相对轨迹
    for(int i = 0; i < relativePathB.size(); i++){
        int x = relativePathB[i].x;
        int y = relativePathB[i].y;
        if(x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE){
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
    for (int x = 0; x < GRID_SIZE; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    for (int y = 0; y < GRID_SIZE; y++) {
        cout << y << " ";
        for (int x = 0; x < GRID_SIZE; x++) {
            cout << grid[y][x] << " ";
        }
        cout << endl;
    }
    
    cout << "图例: A=船A的航迹, R=船B的相对航迹, *=A和R重叠, .=空格" << endl;
}

// 打印网格世界（包含A和B的实际轨迹）
void printActualGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB) {
    char grid[GRID_SIZE][GRID_SIZE];
    
    // 初始化网格
    for(int y = 0; y < GRID_SIZE; y++){
        for (int x = 0; x < GRID_SIZE; x++){
            grid[y][x] = '.';
        }
    }
    
    // 标记A的轨迹
    for(size_t i = 0; i < pathA.size(); i++){
        int x = pathA[i].x;
        int y = pathA[i].y;
        if(x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE){
            grid[y][x] = 'A';
        }
    }
    
    // 标记B的实际轨迹
    for(size_t i = 0; i < pathB.size(); i++){
        int x = pathB[i].x;
        int y = pathB[i].y;
        if(x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE){
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
    for(int x = 0; x < GRID_SIZE; x++){
        cout << x << " ";
    }
    cout << endl;
    
    for(int y = 0; y < GRID_SIZE; y++){
        cout << y << " ";
        for(int x = 0; x < GRID_SIZE; x++){
            cout << grid[y][x] << " ";
        }
        cout << endl;
    }
    
    cout << "图例: A=船A的实际航迹, B=船B的实际航迹, *=A和B重叠, .=空格" << endl;
}

// 生成船只的随机移动轨迹
void generateShipTrajectory(Ship& ship, int steps){
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
        if(newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) { 
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

// 处理玩家输入并验证答案 - 模式1：由B的相对航迹推测B的实际航迹
bool processPlayerInputMode1(const vector<TrajectoryPoint>& correctTrajectory, vector<TrajectoryPoint>& playerAnswer){
    // 开始计时
    auto startTime = chrono::steady_clock::now();
    
    // 获取玩家答案
    cout << "\n请输入B的实际航迹（格式：x1,y1 x2,y2 ...）：" << endl;
    string input;
    getline(cin, input);

    // 结束计时并计算用时
    auto endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    cout << "\n您花费了 " << duration.count() << " 秒完成答题。" << endl;

    // 检查作弊码
    if(input == "999"){
        playerAnswer = correctTrajectory;
        return true;
    }

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
    if (playerAnswer.size() != correctTrajectory.size()) {
        correct = false;
    } 
    else{
        for(size_t i = 0; i < correctTrajectory.size(); i++) {
            if(playerAnswer[i].x != correctTrajectory[i].x || playerAnswer[i].y != correctTrajectory[i].y){
                correct = false;
                break;
            }
        }
    }
    return correct;
}

// 处理玩家输入并验证答案 - 模式2：由B的实际航迹推测B的相对航迹
bool processPlayerInputMode2(const vector<TrajectoryPoint>& correctRelativeTrajectory, vector<TrajectoryPoint>& playerAnswer){
    // 开始计时
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
        playerAnswer = correctRelativeTrajectory;
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
    if(playerAnswer.size() != correctRelativeTrajectory.size()){
        correct = false;
    } 
    else{
        for(size_t i = 0; i < correctRelativeTrajectory.size(); i++){
            if(playerAnswer[i].x != correctRelativeTrajectory[i].x || playerAnswer[i].y != correctRelativeTrajectory[i].y){
                correct = false;
                break;
            }
        }
    }
    return correct;
}

// 处理玩家输入并验证答案 - 模式3：逐步推测B的航迹
bool processPlayerInputMode3(const vector<TrajectoryPoint>& correctTrajectory, vector<TrajectoryPoint>& playerAnswer, bool isRelative){
    cout << "\n请输入B的" << (isRelative ? "相对" : "实际") << "航迹下一步（格式：x,y）：" << endl;
    string input;
    getline(cin, input);

    if (input == "999") {
        playerAnswer.push_back(correctTrajectory.back());
        return true;
    }

    size_t comma = input.find(',');
    int x = stoi(input.substr(0, comma));
    int y = stoi(input.substr(comma+1));
    playerAnswer.push_back(TrajectoryPoint(x, y));
    
    return (playerAnswer.back().x == correctTrajectory[playerAnswer.size()-1].x && 
            playerAnswer.back().y == correctTrajectory[playerAnswer.size()-1].y);
}

// 显示游戏结果 - 模式1：由B的相对航迹推测B的实际航迹
void displayResultMode1(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& playerAnswer) {
    if(correct){
        cout << "正确！B的实际航迹：" << endl;
        printTrajectory(B.trajectory, "B的实际");
        cout << "\n最终位置图示:" << endl;
        // 使用实际轨迹显示函数
        printActualGrid(A.trajectory, B.trajectory);
    } 
    else{
        cout << "错误！正确答案：" << endl;
        printTrajectory(B.trajectory, "B的实际");
        cout << "\n正确的最终位置图示:" << endl;
        // 使用实际轨迹显示函数
        printActualGrid(A.trajectory, B.trajectory);
        
        if(!correct){
            cout << "\n您的答案图示:" << endl;
            // 显示玩家答案和A的实际轨迹
            printActualGrid(A.trajectory, playerAnswer);
        }
    }
}

// 显示游戏结果 - 模式2：由B的实际航迹推测B的相对航迹
void displayResultMode2(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& relativeB, const vector<TrajectoryPoint>& playerAnswer) {
    if(correct){
        cout << "正确！B的相对航迹：" << endl;
        printTrajectory(relativeB, "B的相对");
        cout << "\n最终位置图示:" << endl;
        // 显示A的实际轨迹和B的相对轨迹
        printCombinedGrid(A.trajectory, relativeB);
    } 
    else{
        cout << "错误！正确答案：" << endl;
        printTrajectory(relativeB, "B的相对");
        cout << "\n正确的最终位置图示:" << endl;
        // 显示A的实际轨迹和B的相对轨迹
        printCombinedGrid(A.trajectory, relativeB);
        
        if(!correct){
            cout << "\n您的答案图示:" << endl;
            // 显示A的实际轨迹和玩家的答案
            printCombinedGrid(A.trajectory, playerAnswer);
        }
    }
}

// 显示游戏结果 - 模式3：逐步推测B的航迹
void displayResultMode3(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& relativeB, 
                       const vector<TrajectoryPoint>& playerAnswer, bool isRelative) {
    if(correct){
        cout << "正确！继续下一步。" << endl;
    } 
    else{
        cout << "错误！正确的" << (isRelative ? "相对" : "实际") << "位置应该是：" ;
        if(isRelative) cout << relativeB.back().x << "," << relativeB.back().y << endl;    
        else if(!isRelative) cout << B.trajectory.back().x << "," << B.trajectory.back().y << endl;
    }
}

// 根据网格大小和航迹步数计算难度级别
int calculateDifficulty(int gridSize, int steps){
    int difficulty = 1;
    
    if(steps <= 6){ difficulty = 1; } 
    else if (steps <= 8){ difficulty = 2; } 
    else if (steps <= 10){ difficulty = 3; } 
    else if (steps <= 12){ difficulty = 4; }
    else{ difficulty = 5; }
    return difficulty;
}

// Player类的实现————————————————————————————
// Player类构造函数
Player::Player() : score(0), totalGames(0), successGames(0) {}
Player::Player(const string& name, const string& pwd) 
    : username(name), password(pwd), score(0), totalGames(0), successGames(0) {}

// 获取用户名
string Player::getUsername() const{ return username; }

// 验证密码
bool Player::checkPassword(const string& pwd) const{ return password == pwd; }

// 获取积分
int Player::getScore() const{ return score; }

// 设置分数
void Player::setScore(int newScore){ score = newScore; }

// 增加积分
void Player::addScore(int points){ score += points; }

// 减少积分
void Player::reduceScore(int points){
    if (score >= points) { score -= points; } 
    else { score = 0; }
}

// 玩家管理相关函数声明————————————————————————————————
// 玩家数据文件路径
const string PLAYER_DATA_FILE = "c:\\Cpl\\CPP_Pratical_Training\\player_data.txt";

// 注册新玩家
bool registerPlayer(const string& username, const string& password){
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

// 玩家登录
bool loginPlayer(const string& username, const string& password, Player& player){
    ifstream inFile(PLAYER_DATA_FILE);
    bool userFound = false;
    
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score, total, success;
            ss >> name >> pwd >> score >> total >> success;
            
            if(name == username){
                userFound = true;
                if(pwd == password){
                    player = Player(username, password); // 创建一个新的Player对象表示当前玩家
                    player.setScore(score);
                    // 初始化当前玩家数据
                    for(int i = 0; i < total; i++){
                        player.addGame(i < success);
                    }
                    cout << "登录成功！当前积分：" << score 
                         << " 成功率：" << player.getSuccessRate() << "%" << endl << endl;
                    return true;
                } 
                else {
                    break; // 用户名找到但密码错误，停止搜索
                }
            }
        }
        inFile.close();
        
        if(userFound) {
            cout << "密码错误。" << endl;
        } 
        else {
            cout << "用户名不存在。" << endl;
        }
    } 
    else {
        cout << "无法打开玩家数据文件，登录失败。" << endl;
    }
    return false;
}

// 玩家挑战成功次数
void Player::addGame(bool success){
    totalGames++;
    if(success) successGames++;
}

// 获取玩家总局数
int Player::getTotalGames() const{ return totalGames; }

// 获取玩家挑战成功次数
int Player::getSuccessGames() const{ return successGames; }

// 获取玩家挑战成功概率
double Player::getSuccessRate() const{
    return totalGames > 0 ? (double)successGames / totalGames * 100 : 0;
}

// 保存玩家数据
void savePlayerData(const Player& player){
    vector<string> lines;
    ifstream inFile(PLAYER_DATA_FILE);
    string line;
    
    if(inFile.is_open()){
        while (getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score, total, success;
            ss >> name >> pwd >> score >> total >> success;
            if (name == player.getUsername()) {
                line = name + " " + pwd + " " + to_string(player.getScore()) + " " 
                     + to_string(player.getTotalGames()) + " " 
                     + to_string(player.getSuccessGames());
            }
            lines.push_back(line);
        }
        inFile.close();
        
        // 重新写入文件
        ofstream outFile(PLAYER_DATA_FILE);
        for (const auto& l : lines) {
            outFile << l << endl;
        }
        outFile.close();
    }
    else{
        cout << "无法打开玩家数据文件，数据保存失败" << endl;
    }   
}

// 显示积分排行榜
void displayLeaderboard(){
    vector<tuple<string, int, double>> players;

    ifstream inFile(PLAYER_DATA_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score, total, success;
            ss >> name >> pwd >> score >> total >> success;
            
            double rate = total > 0 ? (double)success / total * 100 : 0;
            players.push_back({name, score, rate});
        }
        inFile.close();
        
        // 按分数降序排序
        sort(players.begin(), players.end(), 
             [](const auto& a, const auto& b){
                 return get<1>(a) > get<1>(b);
             });
        
        cout << "\n===== 积分排行榜 =====" << endl;
        cout << "排名\t用户名\t积分\t成功率" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << get<0>(players[i]) << "\t" 
                 << get<1>(players[i]) << "\t"
                 << fixed << setprecision(1) << get<2>(players[i]) << "%" << endl;
        }
        
        if(players.empty()){
            cout << "暂无玩家数据" << endl;
        }
    }
    else{
        cout << "无法打开玩家数据文件，无法显示排行榜。" << endl;
    }
}

// 根据难度计算得分
int calculatePoints(int difficulty, bool correct) {
    // 根据难度级别设置基础分数
    int Points = difficulty;
    
    if (correct) { return Points * 2; } 
    else { return Points; }
}