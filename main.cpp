#include "Ship.cpp"
#include<iostream>
#include<sstream>
#include<vector>
#include<ctime>
using namespace std;

int main() {
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0)));  
    
    // 生成参照物A的实际轨迹
    auto[aX, aY] = randomStart();
    Ship A(aX, aY);
    generateShipTrajectory(A, 2);  // 移动2步形成3个轨迹点

    // 生成物体B的实际轨迹
    auto [bX, bY] = randomStart();
    Ship B(bX, bY);
    generateShipTrajectory(B, 2);  // 移动2步形成3个轨迹点

    // 计算B相对于A的轨迹
    vector<TrajectoryPoint> relativeB = calculateRelativePath(A.trajectory, B.trajectory);

    // 显示游戏信息
    cout << "===== 脑力航迹游戏 =====" << endl;
    printTrajectory(A.trajectory, "船A的实际");
    printTrajectory(relativeB, "船B的相对");
    
    // 添加可视化显示（同时显示A的实际轨迹和B的相对轨迹）
    cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
    printCombinedGrid(A.trajectory, relativeB);
    
    // 处理玩家输入并验证答案
    vector<TrajectoryPoint> playerAnswer;
    bool correct = processPlayerInput(B.trajectory, playerAnswer);

    // 显示结果
    displayResult(correct, A, B, playerAnswer);

    return 0;
}