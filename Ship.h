#include<vector>
#include<utility>
#include<string>
using namespace std;

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
bool processPlayerInput(const vector<TrajectoryPoint>& correctTrajectory, const vector<TrajectoryPoint>& playerAnswer);

// 显示游戏结果
void displayResult(bool correct, const Ship& A, const Ship& B, const vector<TrajectoryPoint>& playerAnswer);

// 根据网格大小和航迹步数计算难度级别
int calculateDifficulty(int gridSize, int steps);