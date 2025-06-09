#ifndef _MENTAL_TRAJECTORY_H_
#define _MENTAL_TRAJECTORY_H_

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
#include <cmath>
#include <map>
#include <conio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <random>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <limits>
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
private:
    vector<TrajectoryPoint> trajectory;

public:
    Ship(int start_x, int start_y); // 初始化位置
    void move(int dx, int dy); // 移动
    const vector<TrajectoryPoint>& getTrajectory() const { return trajectory; }
};

// 六边形单元格
class HexCell {
private:
    int difficulty;              // 游戏难度 (1-4)
    vector<HexCell*> neighbors;  // 当前单元格的所有邻居指针
    bool isFlag;                 // 是否为旗帜
    bool isStartPoint;           // 是否为起点
    int row, col;                // 位置坐标
    bool isMapCell;              // 是否为地图单元格
    bool hasBeenChoosen;         // 是否已被选择

public:
    HexCell();
    HexCell(int r, int c);                         // 构初始化单元格
    void setDifficulty(int level);                 // 设置单元格难度
    int getDifficulty() const; 
    bool isNeighborWith(HexCell* cell) const;      // 检查是否与另一单元格相邻                    
    void addNeighbor(HexCell* cell);               // 添加相邻单元格
    vector<HexCell*> getNeighbors() const;         // 获取所有相邻单元格
    void setFlag(bool flag);                       // 设置是否为旗帜
    bool getIsFlag() const;                        
    void setStartPoint(bool startPoint);           // 设置是否为起点
    bool getIsStartPoint() const;                 
    pair<int, int> getPosition() const;            // 获取单元格位置坐标
    void setIsMapCell(bool isMap);                 
    bool getIsMapCell() const;
    void setHasBeenChoosen(bool chosen);
    bool getHasBeenChoosen() const;
};

class FunModePlayer{
protected:
    HexCell* currentCell;
    HexCell* startCell;
    bool hasFlag;
    vector<HexCell*> moveHistory; // 移动历史

public:
    FunModePlayer();
    
    void setStartCell(HexCell* cell);
    void moveToCell(HexCell* targetCell);  // 移动到目标单元格
    void moveBackward(); // 后退一步

    virtual bool playGame(int baseGameDifficulty) = 0;  // 进行游戏 // 选择起点
    virtual HexCell* chooseNextMove() = 0;  // 选择下一步

    HexCell* getCurrentCell() const{ return currentCell; }
    bool hasWon() const{ return hasFlag; } // 是否获胜
    const vector<HexCell*>& getMoveHistory() const{ return moveHistory; }
};

// AI玩家
class AIPlayer : public FunModePlayer{
protected:
    int aiDifficulty;  // AI难度级别
    vector<HexCell*> bestPath;

    // AI策略辅助方法
    pair<vector<HexCell*>, int> findBestPath(HexCell* startPoint, HexCell* flagCell);  // 寻找最佳路径
    void setBestPath(const vector<HexCell*>& path);
    vector<HexCell*> getBestPath() const { return bestPath; }
public:
    AIPlayer(int aiDifficulty = 2);         
    bool playGame(int baseGameDifficulty); 
    HexCell* chooseStartCell(const vector<HexCell*>& startCells, HexCell* flagCell, int mapSize);  
    HexCell* chooseNextMove();     
};

class FunPlayer : public FunModePlayer{
protected:
    int funScore, funGames, funSuccessGames;
public:
    FunPlayer();
    bool playGame(int baseGameDifficulty);
    HexCell* chooseStartCell(const vector<HexCell*>& startCells);
    HexCell* chooseNextMove();
    
    void setFunScore(int newScore);
    void addFunScore(int points);
    void addFunGame(bool success);
    void setTotalFunGames(int totalGames);
    void setSuccessFunGames(int successGames);

    int getFunScore() const { return funScore; }
    int getTotalFunGames() const { return funGames; }
    int getSuccessFunGames() const { return funSuccessGames; }
    double getSuccessRate() const;
};

class ChallengePlayer{
protected:
    int challengeScore, challengeGames, challengeSuccessGames; 
    PlayerRank rank;

public:
    ChallengePlayer();

    void setChallengeScore(int newScore);
    void addChallengeScore(int points);
    void reduceChallengeScore(int points);
    void addChallengeGame(bool success);
    void setTotalChallengeGames(int totalChallengeGames);
    void setSuccessChallengeGames(int successChallengeGames);
    void calculateRank();

    int getChallengeScore() const { return challengeScore; }
    int getTotalChallengeGames() const { return challengeGames; }
    int getSuccessChallengeGames() const { return challengeSuccessGames; }
    double getChallengeSuccessRate() const;
    PlayerRank getChallengeRank() const { return rank; }
    string getChallengeRankName() const;
};

class StagePlayer{
protected:
    int currentStage;
    map<int, int> stageTimeRecords;

public:
    StagePlayer();

    void setCurrentStage(int stage);
    void setStageTime(int stage, int timeSeconds);
    
    int getCurrentStage() const { return currentStage; }
    int getStageTime(int stage) const;
    bool hasStageTimeRecord(int stage) const;   
};

class Player : public FunPlayer, public ChallengePlayer, public StagePlayer{
private:
    string username, password;
public:
    Player();
    Player(const string& name, const string& pwd);

    bool checkPassword(const string& pwd) const;
    string getUsername() const{ return username; }
};

class TimeEngine{
private:
    chrono::time_point<chrono::steady_clock> startTime;
    chrono::time_point<chrono::steady_clock> endTime;
    chrono::time_point<chrono::steady_clock> pauseStartTime; 
    chrono::duration<double> pausedDuration; 
    bool isPaused;

public:
    TimeEngine();
    void start();
    void end();
    void pause(); // 暂停计时
    void resume(); // 恢复计时
    void reducePausedDuration(chrono::seconds penaltyTime); // 减少暂停时间 (使用提示时调用)
    void displayTimeCost();

    bool getIsPaused() const { return isPaused; }
    chrono::time_point<chrono::steady_clock> getStartTime() const { return startTime; }
    chrono::time_point<chrono::steady_clock> getEndTime() const { return endTime; }
    chrono::duration<double> getPausedDuration() const { return pausedDuration; }
    chrono::seconds getTimeCost();
};    

class AbstractInitializer; //由于AbstractBaseGmaeMode相关类中需要使用AbstractInitializer类，所以需要先声明

// 游戏模式选择
class AbstractBaseGameMode{
protected:
    bool isStageMode = false; // 标记是否为闯关模式
    bool hintUsed = false; // 是否已使用提示
    TimeEngine timeEngine;

    bool processPlayerInput(vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer, const function<const vector<TrajectoryPoint>&()>& getTrajectory);
    void useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer, const function<const TrajectoryPoint&(size_t)> getSteps);
public:
    virtual ~AbstractBaseGameMode() = default;
    virtual void displayBaseModeInfo(AbstractInitializer* initializer) const = 0;
    virtual void displayGridInfo(AbstractInitializer* initializer) const = 0;
    virtual bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, AbstractInitializer* initializer) = 0;
    virtual void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const = 0;
    
    // 处理时间 
    virtual void timeStart() { timeEngine.start(); }
    virtual void timeEnd() { timeEngine.end(); }
    virtual chrono::seconds getTimeCost() { return timeEngine.getTimeCost(); }
    virtual void displayTimeCost() { timeEngine.displayTimeCost(); }
    virtual void pauseGame() { timeEngine.pause(); }
    virtual void resumeGame() { timeEngine.resume(); }
    virtual bool playBaseGameMode(AbstractInitializer* initializer);

    // 闯关模式相关
    virtual void setIsStageMode(bool isStageMode);
    virtual bool isTimeExceeded(int timeLimit = 150) const; // 检查是否超时，默认限时150秒

    // 提示相关
    virtual void useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer) = 0; // 使用提示
    virtual bool isHintUsed() const { return hintUsed; } // 检查是否已使用提示
    virtual void applyHintTimePenalty() = 0; // 应用提示时间惩罚
};

class BaseGameMode1 : public AbstractBaseGameMode{
private:
    void displayBaseModeInfo(AbstractInitializer* initializer) const;
    void displayGridInfo(AbstractInitializer* initializer) const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, AbstractInitializer* initializer);

    void useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer);
    void applyHintTimePenalty();

public:
    BaseGameMode1();
    ~BaseGameMode1() = default;
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const;
};
class BaseGameMode2 : public AbstractBaseGameMode{
private:
    void displayBaseModeInfo(AbstractInitializer* initializer) const;
    void displayGridInfo(AbstractInitializer* initializer) const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, AbstractInitializer* initializer);
    
    void useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer);
    void applyHintTimePenalty();

public:
    BaseGameMode2();
    ~BaseGameMode2() = default;
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const;
};
class BaseGameMode3 : public AbstractBaseGameMode{
private:
    bool isRelative;

    void setIsRelative();
    void displayBaseModeInfo(AbstractInitializer* initializer) const;
    void displayGridInfo(AbstractInitializer* initializer) const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, AbstractInitializer* initializer);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const;
    
    void useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer);
    void applyHintTimePenalty();

public:
    BaseGameMode3();
    ~BaseGameMode3() = default;
    bool getIsRelative() const;
    bool playBaseGameMode(AbstractInitializer* initializer);
};

class BaseGameModeEngine{   
private:
    int baseGameMode;
    AbstractBaseGameMode* currentBaseGameMode;

    void setBaseGameMode(int stageChoice);
    void randomSetBaseGameMode();
    void setCurrentBaseGameMode(); //多态指针游戏形式指向设置

public:
    BaseGameModeEngine();
    BaseGameModeEngine(int stageChoice);
    ~BaseGameModeEngine();

    bool startBaseGameMode(AbstractInitializer* initializer); // 开始基础游戏形式

    int getBaseGameMode() { return baseGameMode; }
    AbstractBaseGameMode* getCurrentBaseMode() { return currentBaseGameMode; }
    chrono::seconds getTimeCost();
};

class AbstractInitializer{
protected:
    int gridSize, steps, difficulty;
    Ship shipA, shipB;
    vector<TrajectoryPoint> relativeB;
    BaseGameModeEngine baseGameModeEngine;

public:
    AbstractInitializer() : gridSize(0), steps(0), difficulty(0), shipA(0, 0), shipB(0, 0), baseGameModeEngine(){};
    virtual ~AbstractInitializer() = default;
    virtual void generateSteps() = 0;
    virtual void generateGridSize(); // 生成网格大小
    virtual pair<int, int> getRandomStart(); // 生成随机起始位置
    virtual void generateTrajectory(Ship& ship); // 生成船只的随机移动航迹
    virtual void calculateRelativePath(); // 计算相对航迹
    virtual void initializeAllTrajectory(); // 初始化所有航迹

    virtual const int& getGridSize() const { return gridSize; } 
    virtual const int& getSteps() const { return steps; }
    virtual const int& getDifficulty() const { return difficulty; }
    virtual const int& getPoints() const { return difficulty; }
    virtual const Ship& getShipA() const { return shipA; }
    virtual const Ship& getShipB() const { return shipB; }
    virtual const vector<TrajectoryPoint>& getRelativeB() const { return relativeB; }
    virtual BaseGameModeEngine& getBaseGameModeEngine() { return baseGameModeEngine; }
};
class ChallengeInitializer : public AbstractInitializer{
private:
    int maxDifficulty;
    void setMaxDifficulty(Player& player);
public:
    ChallengeInitializer(Player& player);
    ~ChallengeInitializer() = default;
    void generateSteps();
    void calculateDifficulty();
};
class StageInitializer : public AbstractInitializer{
private:
    int stageChoice;
public:
    StageInitializer(Player& player, int _stageChoice);
    ~StageInitializer() = default;
    void generateSteps();
};
class FunInitializer : public AbstractInitializer{
private:
    int baseGameDifficulty;
public:
    FunInitializer(Player& player, int _baseGameDifficulty);
    ~FunInitializer() = default;
    void generateSteps();
};

// 游戏图形显示类
class GameRenderer{
public:
    static void printTrajectory(const vector<TrajectoryPoint>& path, const string& name); // 打印航迹(x,y)
    static void printGrid(const vector<TrajectoryPoint>& ActualPathA, const vector<TrajectoryPoint>& pathB, const AbstractInitializer* initializer, bool isRelative); // 打印网格和航迹
};

// 玩家管理类
class PlayerManager{
private:
    static const string PLAYER_DATA_FILE;

public:
    static bool registerPlayer(const string& username, const string& password); // 注册玩家
    static bool loginPlayer(const string& username, const string& password, Player& player); // 玩家登录
    static void updatePlayerData(const Player& player); // 更新玩家数据
};

// 排行榜管理类
class LeaderboardManager{
private:
    static const string CHALLENGE_LEADERBOARD_FILE;
    static const string STAGE_LEADERBOARD_FILE;
    static const string LEVEL_TIME_FILE;
    static const string FUN_LEADERBOARD_FILE;

public:
    static void updateChallengeLeaderboard(const Player& player);
    static void displayChallengeLeaderboard();

    static void updateStageLeaderboard(const Player& player);
    static void displayStageLeaderboard();
    static void updateStageTimeLeaderboard(const Player& player, int stage, int timeSeconds);
    static void displayStageTimeLeaderboard(int stage);
    static void displayStageTimeLeaderboardMenu();

    static void updateFunLeaderboard(const Player& player);
    static void displayFunLeaderboard();
};

// 挑战模式类
class ChallengeMode{
public:
    static void playChallengeMode(Player& player);
};

// 闯关模式类
class StageMode{
public:
    static void displayAvailableStage(Player& player);
    static void playStageMode(Player& player);
};

// 娱乐模式类
// 六边形地图
class HexMap {
private:
    vector<vector<HexCell>> cells;   // 地图上的所有六边形
    HexCell* flagCell;               // 旗帜所在的单元格
    vector<HexCell*> startPoints;    // 起点列表
    int size;                        // 地图大小

    void setFlag();                  // 设置旗帜位置
    void generateStartPoints();      // 生成起点位置

public:
    HexMap(int gameDifficulty);                           // 创建指定大小的地图
    void generateMap();                            // 生成平衡的地图
    void setBaseDifficulties();                        // 设置各单元格的难度
    void printMap() const;                         // 打印地图信息
    HexCell* getFlagCell() const;                  // 获取旗帜单元格
    vector<HexCell*> getStartCells() const;       // 获取所有起点
    HexCell* getCellAt(int row, int col);          // 获取指定位置的单元格
    int getSize() const;                           // 获取地图大小
    static int calculateDistance(HexCell* cell, HexCell* flagCell, int mapSize);  // 计算两个单元格间的距离
};
class FunModeEngine{
private:
    HexMap gameMap;          
    Player human;      
    AIPlayer ai;          
    bool isHumanTurn;        // 是否为人类回合
    bool gameOver;           // 游戏是否结束
    bool firstRound;         // 是否为第一轮
    int gameDifficulty;      // 游戏难度
    
    void displayGameState();               // 显示游戏状态
    bool processTurn(FunModePlayer* currentPlayer);  // 处理当前玩家回合

public:
    FunModeEngine(Player& player, int gameDifficulty);       
   
    bool startFunMode();        
    void chooseStartPoints(); // 选择起点
    void playRound();         // 进行一轮游戏
    bool isGameOver() const;  // 游戏是否结束
    FunModePlayer* getWinner() const;  // 获取胜利者
};
class FunMode{
public:
    static void playFunMode(Player& player);
};

// 游戏运行类
class GameRunner{
public:
    static void runGame();
};







#endif


