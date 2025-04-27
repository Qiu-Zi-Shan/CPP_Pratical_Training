#include "Mental_Trajectory.h"
#include <iostream>
#include <sstream>
#include <ctime>
using namespace std;

int main(){
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0))); 

    // 玩家登录/注册
    Player currentPlayer;
    bool loggedIn = false;
    
    cout << "===== 欢迎来到脑力航迹游戏 =====" << endl;
    
    while(!loggedIn){
        cout << "1. 登录" << endl;
        cout << "2. 注册" << endl;
        cout << "3. 查看排行榜" << endl;
        cout << "4. 退出" << endl;
        cout << "请选择操作: ";
        
        int choice;
        cin >> choice;
        cin.ignore(); // 清除输入缓冲区
        
        string username, password;
        
        switch(choice){
            case 1: // 登录
                cout << "请输入用户名: ";
                getline(cin, username);
                cout << "请输入密码: ";
                getline(cin, password);
                
                loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                break;
                
            case 2: // 注册
                cout << "请输入用户名: ";
                getline(cin, username);
                cout << "请输入密码: ";
                getline(cin, password);
                
                if(PlayerManager::registerPlayer(username, password)){
                    cout << "是否立即登录? (y/n): ";
                    char loginChoice;
                    cin >> loginChoice;
                    cin.ignore();
                    
                    if(loginChoice == 'y'){
                        loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                    }
                }
                break;
                
            case 3: // 查看排行榜
                PlayerManager::displayLeaderboard();
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
    
    while(playAgain){
        // 创建游戏管理器
        GameManager gameManager;

        // 随机生成网格大小（范围：5-10）
        gameManager.generateGridSize();
        
        // 移动步数
        gameManager.generateSteps();
        
        // 执行游戏，根据结果更新玩家数据
        gameManager.playGameMode(currentPlayer);
        
        // 保存玩家数据
        PlayerManager::savePlayerData(currentPlayer);
        
        // 显示排行榜
        PlayerManager::displayLeaderboard();
        
        // 询问是否继续游戏
        cout << "是否继续游戏? (y/n): \n";
        char choice;
        cin >> choice;
        cin.ignore();
        
        playAgain = (choice == 'y');
    }
    
    cout << "感谢游玩，再见！" << endl;
    return 0;
}