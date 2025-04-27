#ifndef Mental_Trajectory_H
#define Mental_Trajectory_H

#include <vector>
#include <string>
#include <chrono>
#include <utility>
using namespace std;

// 轨迹点结构体
struct TrajectoryPoint{
    int x;
    int y;
    TrajectoryPoint(int s_x, int s_y) : x(s_x), y(s_y) {}
};

// 船只类
class Ship{
public:
    vector<TrajectoryPoint> trajectory;

    Ship(int start_x, int start_y); // 初始化位置
    void move(int dx, int dy); // 移动
};

// 玩家类
class Player{
private:
    string username, password;
    int score, totalGames, successGames; 

public:
    Player();
    Player(const string& name, const string& pwd);

    string getUsername() const;
    bool checkPassword(const string& pwd) const;
    int getScore() const;
    void setScore(int newScore);
    void addScore(int points);
    void reduceScore(int points);
    void addGame(bool success);
    int getTotalGames() const;
    int getSuccessGames() const;
    double getSuccessRate() const;
};

// 游戏管理器类
class GameManager{
    friend class GameRenderer;
private:
    int difficulty, gameMode, gridSize, steps;
    Ship shipA, shipB;
    vector<TrajectoryPoint> relativeB;

public:
    GameManager();
    
    void initializeGame(); // 初始化游戏
    pair<int, int> randomStart(); // 生成随机起始位置
    void generateShipTrajectory(Ship& ship); // 生成船只的随机移动航迹
    void generateRelativeTrajectory(Ship& ship); // 生成船只的相对移动轨迹
    vector<TrajectoryPoint> calculateRelativePath(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB); // 计算相对轨迹
    int calculateDifficulty(); // 根据步数计算难度
    int calculatePoints(int difficulty, bool correct); // 计算得分
    bool playGameMode(Player& player); // 执行游戏模式

    // 处理玩家输入并验证答案
    bool processPlayerInputMode1(vector<TrajectoryPoint>& playerTrajectory); 
    bool processPlayerInputMode2(vector<TrajectoryPoint>& playerTrajectory); 
    bool processPlayerInputMode3(vector<TrajectoryPoint>& playerTrajectory, bool isRelative); 
    
    //显示游戏结果
    void displayResultMode1(bool correct, const vector<TrajectoryPoint>& playerAnswer);
    void displayResultMode2(bool correct, const vector<TrajectoryPoint>& playerAnswer);
    void displayResultMode3(bool correct, const vector<TrajectoryPoint>& playerAnswer, bool isRelative);

    int getDifficulty() const; //获取难度
    int getGameMode() const; //获取游戏模式
    const Ship& getShipA() const; //获取船只A
    const Ship& getShipB() const; //获取船只B
    const vector<TrajectoryPoint>& getRelativeB() const; //获取相对轨迹B
    int getGridSize() const; // 获取网格大小

    int generateGridSize(); // 生成网格大小
    int generateSteps(); // 生成步数
};

// 游戏图形显示类
class GameRenderer{
public:
    static void printTrajectory(const vector<TrajectoryPoint>& path, const string& name); // 打印航迹
    static void printCombinedGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& relativePathB, const GameManager& gameManager); // 打印网格（A实际航迹和B相对航迹）
    static void printActualGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB, const GameManager& gameManager); // 打印网格（A实际航迹和实际航迹）
};

// 玩家管理类
class PlayerManager{
private:
    static const string PLAYER_DATA_FILE;

public:
    static bool registerPlayer(const string& username, const string& password); // 注册玩家
    static bool loginPlayer(const string& username, const string& password, Player& player); // 玩家登录
    static void savePlayerData(const Player& player); // 保存玩家数据
    static void displayLeaderboard(); // 显示排行榜
};

#endif