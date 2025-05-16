#ifndef _MENTAL_TRAJECTORY2_H_
#define _MENTAL_TRAJECTORY2_H_

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
    Ship(int start_x, int start_y); // 初始化位置
    void move(int dx, int dy); // 移动
    const vector<TrajectoryPoint>& getTrajectory() const; 

private:
    vector<TrajectoryPoint> trajectory;
};

// 游戏初始化类
class GameInitializer{

private:
    int gridSize, steps, difficulty;
    Ship shipA, shipB;
    vector<TrajectoryPoint> relativeB;

public:
    GameInitializer();
    
    pair<int, int> getRandomStart(); // 生成随机起始位置
    void generateGridSize(); // 生成网格大小
    void generateSteps(); // 生成步数

    // 游戏游玩
    void generateTrajectory(Ship& ship); // 生成船只的随机移动航迹
    void calculateRelativePath(); // 计算相对轨迹

    // 积分处理
    void calculateDifficulty(); // 根据步数计算难度

    const int& getGridSize() const; 
    const int& getSteps() const;
    const int& getDifficulty() const;
    const int& getPoints() const;
    const Ship& getShipA() const;
    const Ship& getShipB() const;
    const vector<TrajectoryPoint>& getRelativeB() const;
};

class TimeEngine{
    public:
        TimeEngine();
        void start();
        void end();
        chrono::time_point<chrono::steady_clock> getStartTime() const;
        chrono::time_point<chrono::steady_clock> getEndTime() const;
        void displayTimeCost();
    private:
        chrono::time_point<chrono::steady_clock> startTime;
        chrono::time_point<chrono::steady_clock> endTime;
    };    
    
// 游戏模式选择
class AbstractGameMode{
public:
    virtual ~AbstractGameMode() = default;
    virtual void displayModeInfo() const = 0;
    virtual void displayGridInfo() const = 0;
    virtual bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory) = 0;
    virtual void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const = 0;
    virtual void timeStart() = 0;
    virtual void timeEnd() = 0;
    virtual void displayTimeCost() = 0;
    virtual void playGameMode();
    virtual const GameInitializer& getInitializer() const = 0;
};

class GameMode1 : public AbstractGameMode{
public:
    GameMode1();
    void displayModeInfo() const;
    void displayGridInfo() const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const;
    void timeStart();
    void timeEnd();
    void displayTimeCost();
    const GameInitializer& getInitializer() const;

private:
    GameInitializer initializer;
    TimeEngine timeEngine;  
};

class GameMode2 : public AbstractGameMode{
public:
    GameMode2();
    void displayModeInfo() const;
    void displayGridInfo() const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const;
    void timeStart();
    void timeEnd();
    void displayTimeCost();
    const GameInitializer& getInitializer() const;

private:
    GameInitializer initializer;
    TimeEngine timeEngine;
};

class GameMode3 : public AbstractGameMode{
public:
    GameMode3();
    void setIsRelative();
    bool getIsRelative() const;
    void displayModeInfo() const;
    void displayGridInfo() const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const;
    void playGameMode();
    void timeStart();
    void timeEnd();
    void displayTimeCost();
    const GameInitializer& getInitializer() const;

private:
    bool isRelative;
    GameInitializer initializer;
    TimeEngine timeEngine;
};

class GameMode{
public:
    GameMode();
    ~GameMode();
    AbstractGameMode* getCurrentMode();
    void beginBaseGameMode(); // 开始基础游戏形式

private:
    AbstractGameMode* currentGameMode;
    void setCurrentGameMode();
};

// 游戏图形显示类
class GameRenderer{
public:
    static void printTrajectory(const vector<TrajectoryPoint>& path, const string& name); // 打印航迹(x,y)
    static void printGrid(const vector<TrajectoryPoint>& ActualPathA, const vector<TrajectoryPoint>& pathB, const GameInitializer& initializer, bool isRelative); // 打印网格和航迹
};
    
class BaseGameRun{
private:
    GameMode gameMode;
public:
    BaseGameRun();
    void run();
};

#endif


