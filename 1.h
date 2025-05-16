#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <utility>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <tuple>
#include <ctime>
using namespace std;

// 轨迹点结构体
struct TrajectoryPoint{
    int x;
    int y;
    TrajectoryPoint(int s_x, int s_y) : x(s_x), y(s_y) {}
};

// 玩家等级枚举
enum class PlayerRank{
    BRONZE, // 青铜
    SILVER, // 白银
    GOLD, // 黄金
    PLATINUM, // 铂金
    DIAMOND, // 钻石
    MASTER // 大师
};

// 船只类
class Ship{
public:
    vector<TrajectoryPoint> trajectory;
    
    Ship(int start_x, int start_y); // 初始化位置
    void move(int dx, int dy); // 移动
};

// 游戏初始化类
class GameInitializer{
private:
    int gridSize, steps, difficulty;
    Ship shipA, shipB;
    vector<TrajectoryPoint> relativeB;

public:
    GameInitializer();
    
    pair<int, int> randomStart(); // 生成随机起始位置
    void generateGridSize(); // 生成网格大小
    void generateSteps(); // 生成步数

    // 游戏游玩
    void generateTrajectory(Ship& ship); // 生成船只的随机移动航迹
    void calculateRelativePath(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB); // 计算相对轨迹

    // 积分处理
    void calculateDifficulty(); // 根据步数计算难度

    int getGridSize() const; 
    int getSteps() const;
    int getDifficulty() const;
    int getPoints() const;
    const Ship& getShipA() const;
    const Ship& getShipB() const;
    const vector<TrajectoryPoint>& getRelativeB() const;
};

// 游戏模式选择
class AbstractGameMode{
public:
    virtual bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory) = 0;
    virtual void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, bool isRelative) = 0;
};

class GameMode1 : public AbstractGameMode{
public:
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer);
};

class GameMode2 : public AbstractGameMode{
public:
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer);
};

class GameMode3 : public AbstractGameMode{
public:
    GameMode3();
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer);
    
private:
    bool processPlayerInput3(const vector<TrajectoryPoint>& playerTrajectory, bool isRelative);
    void displayResultMode3(bool correct, const vector<TrajectoryPoint>& playerAnswer, bool isRelative);
};

// 游戏图形显示类
class GameRenderer{
public:
    static void printTrajectory(const vector<TrajectoryPoint>& path, const string& name); // 打印航迹(x,y)
    static void printGrid(const vector<TrajectoryPoint>& ActualPathA, const vector<TrajectoryPoint>& pathB, const GameManager& gameManager); // 打印网格和航迹
};

class GameMode{
private:
    int gameMode;
    Abstract GameMode* currentGameMode;
public:
    void setCurrentGameMode(bool isRelative = false);
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer);
    int getGameMode() const;
};

class BaseGameRun(){
private:
    GameInitializer initializer;
    GameMode gameMode;
public:
    BaseGameRun();
    void run();
}

