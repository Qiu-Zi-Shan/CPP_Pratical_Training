#include "Ship.cpp"
#include<iostream>
#include<sstream>
#include<vector>
#include<ctime>
using namespace std;

int GRID_SIZE;

int main() {
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0)));  
    
    // 随机生成网格大小（范围：5-10）
    GRID_SIZE = 5 + rand() % 6;
    cout << "网格大小: " << GRID_SIZE << "x" << GRID_SIZE << endl;

    // 移动步数
    int steps = GRID_SIZE + rand() % (GRID_SIZE / 2);  // 根据网格大小，随机生成航迹步数
    cout << "航迹步数: " << steps << endl;

    // 随机选择游戏模式
    bool isMode1 = (rand() % 2 == 0);  // 随机选择模式1或模式2
    
    if (isMode1) {
        cout << "游戏模式：由B的相对航迹推测B的实际航迹" << endl;
    } 
    else {
        cout << "游戏模式：由B的实际航迹推测B的相对航迹" << endl;
    }

    // 生成参照物A的实际轨迹
    auto[aX, aY] = randomStart();
    Ship A(aX, aY);
    generateShipTrajectory(A, steps);  

    // 生成物体B的实际轨迹
    auto [bX, bY] = randomStart();
    Ship B(bX, bY);
    generateShipTrajectory(B, steps); 

    // 计算B相对于A的轨迹
    vector<TrajectoryPoint> relativeB = calculateRelativePath(A.trajectory, B.trajectory);

    // 显示游戏信息
    cout << "===== 脑力航迹游戏 =====" << endl;
    
    if (isMode1) {
        // 模式1：显示A的实际轨迹和B的相对轨迹
        printTrajectory(A.trajectory, "船A的实际");
        printTrajectory(relativeB, "船B的相对");
        
        // 添加可视化显示
        cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
        printCombinedGrid(A.trajectory, relativeB);
        
        // 处理玩家输入并验证答案
        vector<TrajectoryPoint> playerAnswer;
        bool correct = processPlayerInputMode1(B.trajectory, playerAnswer);

        // 显示结果
        displayResultMode1(correct, A, B, playerAnswer);
    } 
    else {
        // 模式2：显示A的实际轨迹和B的实际轨迹
        printTrajectory(A.trajectory, "船A的实际");
        printTrajectory(B.trajectory, "船B的实际");
        
        // 添加可视化显示
        cout << "\n参照物A和物体B的实际位置:" << endl;
        printActualGrid(A.trajectory, B.trajectory);
        
        // 处理玩家输入并验证答案
        vector<TrajectoryPoint> playerAnswer;
        bool correct = processPlayerInputMode2(relativeB, playerAnswer);

        // 显示结果
        displayResultMode2(correct, A, B, relativeB, playerAnswer);
    }

    return 0;
}