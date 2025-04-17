#include<vector>
#include<utility>
#include<string>
using namespace std;

// 游戏实现————————————————————
extern int GRID_SIZE;  // 方格世界大小

#ifndef TRAJECTORYPOINT_H
#define TRAJECTORYPOINT_H

// 轨迹点结构体
struct TrajectoryPoint{
    int x;
    int y;
    TrajectoryPoint(int s_x, int s_y) : x(s_x), y(s_y) {}
};   

#endif 

#ifndef SHIP_H
#define SHIP_H

class Ship{
public:
    vector<TrajectoryPoint> trajectory;

    Ship(int startX, int startY);  // 初始化位置
    void move(int dx, int dy); // 添加移动轨迹
};

#endif

// 生成随机起始位置
pair<int, int> randomStart();

// 打印轨迹
void printTrajectory(const vector<TrajectoryPoint>& path, const string& name);

// 计算相对轨迹（以B的起始位置为起点，但表示相对于A的运动）
vector<TrajectoryPoint> calculateRelativePath(const vector<TrajectoryPoint>& base,const vector<TrajectoryPoint>& target);

// 打印网格世界（包含A的实际轨迹和B的相对轨迹）
void printCombinedGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& relativePathB);

// 打印网格世界（包含A和B的实际轨迹）
void printActualGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB);

// 生成船只的随机移动轨迹
void generateShipTrajectory(Ship& ship, int steps);

// 处理玩家输入并验证答案
bool processPlayerInputMode1(const vector<TrajectoryPoint>& correctTrajectory, vector<TrajectoryPoint>& playerAnswer);
bool processPlayerInputMode2(const vector<TrajectoryPoint>& correctRelativeTrajectory, vector<TrajectoryPoint>& playerAnswer);
bool processPlayerInputMode3(const vector<TrajectoryPoint>& correctTrajectory, vector<TrajectoryPoint>& playerAnswer, bool isRelative);
// 显示游戏结果
void displayResultMode1(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& playerAnswer);
void displayResultMode2(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& relativeB, const vector<TrajectoryPoint>& playerAnswer);

// 显示游戏结果 - 模式3
void displayResultMode3(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& relativeB, const vector<TrajectoryPoint>& playerAnswer, bool isRelative);

// 根据网格大小和航迹步数计算难度级别
int calculateDifficulty(int gridSize, int steps);

//玩家类————————————————————————————————————————
#ifndef PLAYER_H
#define PLAYER_H

class Player {
private:
    string username;
    string password;
    int score;
    int totalGames;    
    int successGames; 

public:
    Player();
    Player(const string& name, const string& pwd);
    
    // 获取用户名
    string getUsername() const;
    
    // 验证密码
    bool checkPassword(const string& pwd) const;
    
    // 获取积分
    int getScore() const;
    
    // 设置积分
    void setScore(int newScore);
    
    // 增加积分
    void addScore(int points);
    
    // 减少积分
    void reduceScore(int points);
    
    void addGame(bool success);
    int getTotalGames() const;
    int getSuccessGames() const;
    double getSuccessRate() const;
};
#endif

// 玩家管理相关函数声明——————————————————————————
// 注册新玩家
bool registerPlayer(const string& username, const string& password);

// 玩家登录
bool loginPlayer(const string& username, const string& password, Player& player);

// 保存玩家数据
void savePlayerData(const Player& player);

// 显示积分排行榜
void displayLeaderboard();
int calculatePoints(int difficulty, bool correct);