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
#include <cmath>
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

// 玩家类
class Player{
private:
    string username, password;
    int challengeScore, challengeGames, challengeSuccessGames; 
    int currentStage;
    PlayerRank rank;

public:
    Player();
    Player(const string& name, const string& pwd);

    bool checkPassword(const string& pwd) const;
    string getUsername() const;

    // 挑战模式相关
    void setChallengeScore(int newScore);
    void addChallengeScore(int points);
    void reduceChallengeScore(int points);
    void setTotalChallengeGames(int totalChallengeGames);
    void setSuccessChallengeGames(int successChallengeGames);
    void addChallengeGame(bool success);
    void calculateRank();

    int getChallengeScore() const;
    int getTotalChallengeGames() const;
    int getSuccessChallengeGames() const;
    double getChallengeSuccessRate() const;
    PlayerRank getChallengeRank() const;
    string getChallengeRankName() const;

    // 闯关模式相关
    void setCurrentStage(int stage);
    int getCurrentStage() const;
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

class GameInitializer; //由于AbstractBaseGmaeMode相关类中需要使用GameInitializer类，所以需要先声明

// 游戏模式选择
class AbstractBaseGameMode{
public:
    virtual ~AbstractBaseGameMode() = default;
    virtual void displayBaseModeInfo(GameInitializer& initializer) const = 0;
    virtual void displayGridInfo(GameInitializer& initializer) const = 0;
    virtual bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, GameInitializer& initializer) = 0;
    virtual void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const = 0;
    virtual void timeStart() = 0;
    virtual void timeEnd() = 0;
    virtual void displayTimeCost() = 0;
    virtual bool playBaseGameMode(GameInitializer& initializer);
};

class BaseGameMode1 : public AbstractBaseGameMode{
public:
    BaseGameMode1();
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const;

private:
    TimeEngine timeEngine;  

    void displayBaseModeInfo(GameInitializer& initializer) const;
    void displayGridInfo(GameInitializer& initializer) const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, GameInitializer& initializer);
    void timeStart();
    void timeEnd();
    void displayTimeCost();
};

class BaseGameMode2 : public AbstractBaseGameMode{
public:
    BaseGameMode2();
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const;

private:
    TimeEngine timeEngine;

    void displayBaseModeInfo(GameInitializer& initializer) const;
    void displayGridInfo(GameInitializer& initializer) const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, GameInitializer& initializer);
    void timeStart();
    void timeEnd();
    void displayTimeCost();
};

class BaseGameMode3 : public AbstractBaseGameMode{
public:
    BaseGameMode3();
    bool getIsRelative() const;
    bool playBaseGameMode(GameInitializer& initializer);
    
private:
    bool isRelative;
    TimeEngine timeEngine;

    void setIsRelative();
    void displayBaseModeInfo(GameInitializer& initializer) const;
    void displayGridInfo(GameInitializer& initializer) const;
    bool processPlayerInput(vector<TrajectoryPoint>& playerTrajectory, GameInitializer& initializer);
    void displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const;
    void timeStart();
    void timeEnd();
    void displayTimeCost();
};

class BaseGameModeEngine{
public:
    BaseGameModeEngine();
    BaseGameModeEngine(int stageChoice);
    ~BaseGameModeEngine();

    int getBaseGameMode();
    bool startBaseGameMode(GameInitializer& initializer); // 开始基础游戏形式

private:
    int baseGameMode;
    AbstractBaseGameMode* currentBaseGameMode;

    void setBaseGameMode(int stageChoice);
    void randomSetBaseGameMode();
    void setCurrentBaseGameMode(); //多态指针游戏形式指向设置

    AbstractBaseGameMode* getCurrentBaseMode();
};

// 游戏初始化类
class GameInitializer{
private:
    int gridSize, steps, difficulty;
    Ship shipA, shipB;
    vector<TrajectoryPoint> relativeB;
    int maxDifficulty;
    BaseGameModeEngine baseGameModeEngine;

    //积分挑战模式相关
    void setChallengeMaxDifficulty(Player& player);
    void generateChallengeSteps(); // 生成步数
    void calculateChallengeDifficulty(); // 计算难度

    // 闯关模式相关
    void generateStageSteps(int stageChoice); // 生成步数

    void generateGridSize(); // 生成网格大小
    pair<int, int> getRandomStart(); // 生成随机起始位置
    void generateTrajectory(Ship& ship); // 生成船只的随机移动航迹
    void calculateRelativePath(); // 计算相对航迹
    void initializeAllTrajectory(); // 初始化所有航迹

public:
    GameInitializer(Player& player);
    GameInitializer(int stageChoice, Player& player);

    const int& getGridSize() const; 
    const int& getSteps() const;
    const int& getDifficulty() const;
    const int& getPoints() const;
    const Ship& getShipA() const;
    const Ship& getShipB() const;
    const vector<TrajectoryPoint>& getRelativeB() const;
    int getChallengeMaxDifficulty() const;
    BaseGameModeEngine& getBaseGameModeEngine();
};

// 游戏图形显示类
class GameRenderer{
public:
    static void printTrajectory(const vector<TrajectoryPoint>& path, const string& name); // 打印航迹(x,y)
    static void printGrid(const vector<TrajectoryPoint>& ActualPathA, const vector<TrajectoryPoint>& pathB, const GameInitializer& initializer, bool isRelative); // 打印网格和航迹
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

public:
    static void updateChallengeLeaderboard(const Player& player);
    static void displayChallengeLeaderboard();

    static void updateStageLeaderboard(const Player& player);
    static void displayStageLeaderboard();
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

// 游戏运行类
class GameRunner{
public:
    static void runGame();
};






#endif


