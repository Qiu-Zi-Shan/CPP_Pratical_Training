#include "Ship.h"
#include<iostream>
#include<sstream>
#include<vector>
#include<ctime>
#include<algorithm>
#include<fstream>
#include<chrono>
using namespace std;

int GRID_SIZE;

int main() {
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0)));  
    
    // 玩家登录/注册
    Player currentPlayer;
    bool loggedIn = false;
    
    cout << "===== 欢迎来到脑力航迹游戏 =====" << endl;
    
    while (!loggedIn) {
        cout << "1. 登录" << endl;
        cout << "2. 注册" << endl;
        cout << "3. 查看排行榜" << endl;
        cout << "4. 退出" << endl;
        cout << "请选择操作: ";
        
        int choice;
        cin >> choice;
        cin.ignore(); // 清除输入缓冲区
        
        string username, password;
        
        switch (choice) {
            case 1: // 登录
                cout << "请输入用户名: ";
                getline(cin, username);
                cout << "请输入密码: ";
                getline(cin, password);
                
                loggedIn = loginPlayer(username, password, currentPlayer);
                break;
                
            case 2: // 注册
                cout << "请输入用户名: ";
                getline(cin, username);
                cout << "请输入密码: ";
                getline(cin, password);
                
                if (registerPlayer(username, password)) {
                    cout << "是否立即登录? (y/n): ";
                    char loginChoice;
                    cin >> loginChoice;
                    cin.ignore();
                    
                    if (loginChoice == 'y') {
                        loggedIn = loginPlayer(username, password, currentPlayer);
                    }
                }
                break;
                
            case 3: // 查看排行榜
                displayLeaderboard();
                break;
                
            case 4: // 退出
                cout << "感谢使用，再见！" << endl;
                return 0;
                
            default:
                cout << "无效选择，请重试。" << endl;
        }
    }
    
    // 游戏主循环
    bool playAgain = true;
    
    while (playAgain) {
        // 随机生成网格大小（范围：5-10）
        GRID_SIZE = 5 + rand() % 6;
        cout << "网格大小: " << GRID_SIZE << "x" << GRID_SIZE << endl;
    
        // 移动步数
        int steps = GRID_SIZE + rand() % (GRID_SIZE / 2);  // 根据网格大小，随机生成航迹步数
        cout << "航迹步数: " << steps << endl;
    
        // 计算并显示难度级别
        int difficulty = calculateDifficulty(GRID_SIZE, steps);
        cout << "本次游戏难度级别: " << difficulty << "级 (1级最简单)" << endl;
    
        // 随机选择游戏模式
        int modeChoice = rand() % 3;  // 0, 1, 2 三种模式
        
        if (modeChoice == 0) {
            cout << "游戏模式：由B的相对航迹推测B的实际航迹" << endl;
        } 
        else if (modeChoice == 1) {
            cout << "游戏模式：由B的实际航迹推测B的相对航迹" << endl;
        }
        else {
            cout << "游戏模式：逐步推测B的航迹" << endl;
            difficulty = difficulty + 2;  // 难度上升两级
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
        
        bool correct = false;
        vector<TrajectoryPoint> playerAnswer;
        
        if (modeChoice == 0) {
            // 模式1：显示A的实际轨迹和B的相对轨迹
            printTrajectory(A.trajectory, "船A的实际");
            printTrajectory(relativeB, "船B的相对");
            
            cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
            printCombinedGrid(A.trajectory, relativeB);
            
            correct = processPlayerInputMode1(B.trajectory, playerAnswer);
            displayResultMode1(correct, A, B, playerAnswer);
        } 
        else if (modeChoice == 1) {
            // 模式2：显示A的实际轨迹和B的实际轨迹
            printTrajectory(A.trajectory, "船A的实际");
            printTrajectory(B.trajectory, "船B的实际");
            
            cout << "\n参照物A和物体B的实际位置:" << endl;
            printActualGrid(A.trajectory, B.trajectory);
            
            correct = processPlayerInputMode2(relativeB, playerAnswer);
            displayResultMode2(correct, A, B, relativeB, playerAnswer);
        }
        else {
            // 模式3：逐步显示和验证
            vector<TrajectoryPoint> playerAnswer;
            correct = true;
            
            // 开始计时
            auto startTime = chrono::steady_clock::now();

            for (size_t step = 0; step < A.trajectory.size() && correct; ++step) {
                // 随机决定这一步是猜实际航迹还是相对航迹
                bool isRelative = rand() % 2;
                
                // 显示A的当前轨迹
                vector<TrajectoryPoint> currentA(A.trajectory.begin(), A.trajectory.begin() + step + 1);
                printTrajectory(currentA, "船A的实际");
                
                // 准备当前步骤的轨迹
                vector<TrajectoryPoint> currentB(B.trajectory.begin(), B.trajectory.begin() + step + 1);
                vector<TrajectoryPoint> currentRelativeB(relativeB.begin(), relativeB.begin() + step + 1);
                
                if (!isRelative) {
                    // 如果要猜B的实际航迹，显示完整的相对航迹，但实际航迹少显示一步
                    printTrajectory(currentRelativeB, "船B的相对");
                    if (step > 0) {
                        vector<TrajectoryPoint> prevB(B.trajectory.begin(), B.trajectory.begin() + step);
                        printTrajectory(prevB, "船B的实际");
                    }
                    cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
                    printCombinedGrid(currentA, currentRelativeB);
                } else {
                    // 如果要猜B的相对航迹，显示完整的实际航迹，但相对航迹少显示一步
                    printTrajectory(currentB, "船B的实际");
                    if (step > 0) {
                        vector<TrajectoryPoint> prevRelativeB(relativeB.begin(), relativeB.begin() + step);
                        printTrajectory(prevRelativeB, "船B的相对");
                    }
                    cout << "\n参照物A的实际位置和物体B的实际航迹:" << endl;
                    printActualGrid(currentA, currentB);
                }

                // 处理玩家输入
                if (!isRelative) {
                    // 如果要猜B的实际航迹，需要与B的实际航迹比较
                    correct = processPlayerInputMode3(B.trajectory, playerAnswer, isRelative);
                } else {
                    // 如果要猜B的相对航迹，需要与B的相对航迹比较
                    correct = processPlayerInputMode3(relativeB, playerAnswer, isRelative);
                }

                // 显示当前步骤结果
                displayResultMode3(correct, A, B, relativeB, playerAnswer, isRelative);

                // 切换下一步是显示相对还是实际航迹
                isRelative = !isRelative;
            }
            
            // 结束计时并显示总用时
            auto endTime = chrono::steady_clock::now();
            auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
            if (correct) {
                cout << "\n恭喜完成挑战！总用时：" << duration.count() << " 秒。" << endl;
            } else {
                cout << "\n挑战失败。本次用时：" << duration.count() << " 秒。" << endl;
            }
        }

        // 更新玩家积分和游戏记录
        int points = calculatePoints(difficulty, correct);
        currentPlayer.addGame(correct);  // 添加这行
        if (correct) {
            cout << "恭喜！你获得了 " << points << " 分！" << endl;
            currentPlayer.addScore(points);
        }
        else {
            if (currentPlayer.getScore() > points) {
                cout << "很遗憾，你失去了 " << points << " 分。" << endl;
                currentPlayer.reduceScore(points);
            }
            else if (currentPlayer.getScore() > 0) {
                cout << "很遗憾，你失去了 " << currentPlayer.getScore() << " 分。" << endl;
                currentPlayer.setScore(0);
            }
            else {
                cout << "很遗憾，答错了。但你的分数已经为0，不再扣分。" << endl;
            }
        }
        
        cout << "当前积分: " << currentPlayer.getScore() << endl;
        
        // 保存玩家数据
        savePlayerData(currentPlayer);
        
        // 显示排行榜
        displayLeaderboard();
        
        // 询问是否继续游戏
        cout << "是否继续游戏? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();
        
        playAgain = (choice == 'y');
    }
    
    cout << "感谢游玩，再见！" << endl;
    return 0;
}