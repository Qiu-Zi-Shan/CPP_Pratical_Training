#ifndef Mental_Trajectory_H
#define Mental_Trajectory_H

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

// 关卡定义结构体
struct Level{
    int levelNumber;
    int gridSize;
    int steps;
    int gameMode;
    int timeLimit; 
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

// 玩家类
class Player{
private:
    string username, password;
    int score, totalGames, successGames, challengeScore, challengeGames, challengeSuccessGames; 
    PlayerRank rank;

public:
    Player();
    Player(const string& name, const string& pwd);

    string getUsername() const;
    bool checkPassword(const string& pwd) const;

    // 娱乐模式分数相关
    int getScore() const;
    int getTotalGames() const;
    int getSuccessGames() const;
    double getSuccessRate() const;
    void setScore(int newScore);
    void addScore(int points);
    void reduceScore(int points);
    void addGame(bool success);
    
    // 挑战模式分数相关
    int getChallengeScore() const;
    int getTotalChallengeGames() const;
    int getSuccessChallengeGames() const;
    double getChallengeSuccessRate() const;
    void setChallengeScore(int newScore);
    void addChallengeScore(int points);
    void reduceChallengeScore(int points);
    void addChallengeGame(bool success);

    // 获取等级（仅挑战模式）
    PlayerRank getChallengeRank() const;
    string getChallengeRankName() const;
    static PlayerRank calculateRank(int score);
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
    int generateGridSize(); // 生成网格大小
    int generateSteps(); // 生成步数
    void generateShipTrajectory(Ship& ship); // 生成船只的随机移动航迹
    void generateRelativeTrajectory(Ship& ship); // 生成船只的相对移动轨迹
    vector<TrajectoryPoint> calculateRelativePath(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB); // 计算相对轨迹
    int calculateDifficulty(); // 根据步数计算难度
    int calculatePoints(int difficulty, bool correct); // 计算得分
    bool playGameMode(Player& player, bool isChallenge = false);  // 支持两种模式

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

    void setupForLevel(const Level& level); // 设置关卡参数
    void setMaxDifficulty(int max); // 设置难度上限
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
    static void displayLeaderboard(); // 娱乐模式排行榜
};

// 挑战积分模式类
class ChallengeMode{
private:
    GameManager gameManager;
    int maxDifficulty; // 基于玩家等级的最大难度
    
public:
    ChallengeMode(PlayerRank rank);
    bool playChallenge(Player& player);
    static int getMaxDifficultyForRank(PlayerRank rank);

    static void displayChallengeLeaderboard();
};

// 闯关模式类
class AdventureMode{
private:
    static const vector<Level> levels;
    int currentLevel;
    GameManager gameManager;
    
public:
    AdventureMode();
    bool playLevel(Player& player, int levelNumber);
    int getCurrentLevel() const;
    static int getLevelCount();
        
    // 获取玩家已解锁的最高关卡
    static int getHighestUnlockedLevel(const string& username);

    static void displayLeaderboardOptions(int levelNumber);
    static void displayLevelLeaderboard(int levelNumber);
    static void displayOverallLeaderboard();
    static void saveLevelTime(const string& username, int levelNumber, int timeSeconds);
};

// 排行榜管理类
class LeaderboardManager{
private:
    static const string CHALLENGE_LEADERBOARD_FILE;
    static const string ADVENTURE_LEADERBOARD_FILE;
    static const string LEVEL_TIME_FILE;

public:
    static void saveChallengeScore(const Player& player);
    static void saveAdventureProgress(const string& username, int level);
    static void saveLevelTime(const string& username, int level, int timeSeconds);
    
    static void displayChallengeLeaderboard();
    static void displayAdventureLeaderboard();
    static void displayLevelTimeLeaderboard(int level);
};

#endif