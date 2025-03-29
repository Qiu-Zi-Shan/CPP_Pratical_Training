#include <iostream>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <sstream>
using namespace std;

const int GRID_SIZE = 5;  // 5x5的方格世界

// 轨迹点结构体
struct TrajectoryPoint {
    int x;
    int y;
    TrajectoryPoint(int x_, int y_) : x(x_), y(y_) {}
};

// 船只类
class Ship {
public:
    vector<TrajectoryPoint> trajectory;
    
    // 初始化位置
    Ship(int startX, int startY) {
        trajectory.emplace_back(startX, startY);
    }

    // 添加移动轨迹
    void move(int dx, int dy) {
        TrajectoryPoint last = trajectory.back();
        trajectory.emplace_back(last.x + dx, last.y + dy);
    }
};

// 生成随机起始位置
pair<int, int> randomStart() {
    return {rand() % GRID_SIZE, rand() % GRID_SIZE};
}

// 打印轨迹
void printTrajectory(const vector<TrajectoryPoint>& path, const string& name) {
    cout << name << "航迹：";
     for (const TrajectoryPoint& p : path) {
        cout << "(" << p.x << "," << p.y << ") ";
    }
    cout << endl;
}

// 计算相对轨迹（以B的起始位置为起点，但表示相对于A的运动）
vector<TrajectoryPoint> calculateRelativePath(const vector<TrajectoryPoint>& base,const vector<TrajectoryPoint>& target) {
    vector<TrajectoryPoint> relative;

    // 第一个点是B的起始位置
    relative.emplace_back(target[0].x, target[0].y);
    
    // 计算每一步A的位移，并将其反向应用到B的位置上
     int currentX = target[0].x;
     int currentY = target[0].y;
        
    for (size_t i = 1; i < target.size(); i++) {
        // B的实际位移
        int bDx = target[i].x - target[i-1].x;
        int bDy = target[i].y - target[i-1].y;
            
        // A的实际位移（如果A和B的轨迹长度不同，需要处理边界情况）
        int aDx = (i < base.size()) ? (base[i].x - base[i-1].x) : 0;
        int aDy = (i < base.size()) ? (base[i].y - base[i-1].y) : 0;
            
        // 从A的视角看，B的相对位移 = B的实际位移 - A的实际位移
        int relDx = bDx - aDx;
        int relDy = bDy - aDy;
            
        // 累加相对位移
        currentX += relDx;
        currentY += relDy;
            
        relative.emplace_back(currentX, currentY);
    }
    return relative;
}

// 打印网格世界（包含A的实际轨迹和B的相对轨迹）
void printCombinedGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& relativePathB) {
    char grid[GRID_SIZE][GRID_SIZE];
    
    // 初始化网格
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x] = '.';
        }
    }
    
    // 标记A的轨迹
    for (int i = 0; i < pathA.size(); i++) {
        int x = pathA[i].x;
        int y = pathA[i].y;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            grid[y][x] = 'A';
        }
    }
    
    // 标记B的相对轨迹
    for (int i = 0; i < relativePathB.size(); i++) {
        int x = relativePathB[i].x;
        int y = relativePathB[i].y;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            // 如果与A重叠，用'*'表示
            if (grid[y][x] == 'A') {
                grid[y][x] = '*';
            } 
            else {
                grid[y][x] = 'R'; // R表示相对轨迹
            }
        }
    }
    
    // 打印网格
    cout << "  ";
    for (int x = 0; x < GRID_SIZE; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    for (int y = 0; y < GRID_SIZE; y++) {
        cout << y << " ";
        for (int x = 0; x < GRID_SIZE; x++) {
            cout << grid[y][x] << " ";
        }
        cout << endl;
    }
    
    cout << "图例: A=船A的航迹, R=船B的相对航迹, *=A和R重叠, .=空格" << endl;
}

// 打印网格世界（包含A和B的实际轨迹）
void printActualGrid(const vector<TrajectoryPoint>& pathA, const vector<TrajectoryPoint>& pathB) {
    char grid[GRID_SIZE][GRID_SIZE];
    
    // 初始化网格
    for (int y = 0; y < GRID_SIZE; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[y][x] = '.';
        }
    }
    
    // 标记A的轨迹
    for (size_t i = 0; i < pathA.size(); i++) {
        int x = pathA[i].x;
        int y = pathA[i].y;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            grid[y][x] = 'A';
        }
    }
    
    // 标记B的实际轨迹
    for (size_t i = 0; i < pathB.size(); i++) {
        int x = pathB[i].x;
        int y = pathB[i].y;
        if (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE) {
            // 如果与A重叠，用'*'表示
            if (grid[y][x] == 'A') {
                grid[y][x] = '*';
            } else {
                grid[y][x] = 'B'; // B表示B的实际轨迹
            }
        }
    }
    
    // 打印网格
    cout << "  ";
    for (int x = 0; x < GRID_SIZE; x++) {
        cout << x << " ";
    }
    cout << endl;
    
    for (int y = 0; y < GRID_SIZE; y++) {
        cout << y << " ";
        for (int x = 0; x < GRID_SIZE; x++) {
            cout << grid[y][x] << " ";
        }
        cout << endl;
    }
    
    cout << "图例: A=船A的实际航迹, B=船B的实际航迹, *=A和B重叠, .=空格" << endl;
}

int main() {
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0)));  
    
    // 生成参照物A的实际轨迹
    auto[aX, aY] = randomStart();
    Ship A(aX, aY);
    
    // 定义可能的移动方向：上、右、下、左
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    for (int i = 0; i < 2; i++) {  // 移动2步形成3个轨迹点
        // 随机选择一个方向（0-3）
        int direction = rand() % 4;
        
        // 如果是第二步，确保不会往回走
        if (i > 0) {
            TrajectoryPoint prev = A.trajectory[A.trajectory.size() - 2];
            TrajectoryPoint curr = A.trajectory.back();
            
            // 计算上一步的方向
            int lastDx = curr.x - prev.x;
            int lastDy = curr.y - prev.y;
            
            // 避免选择相反的方向
            while ((dx[direction] == -lastDx && dy[direction] == -lastDy)) {
                direction = rand() % 4;
            }
        }
        
        // 确保A不会在原地停留（dx和dy不能同时为0）
        while (dx[direction] == 0 && dy[direction] == 0) {
            direction = rand() % 4;
        }
        
        // 检查移动后是否会超出边界，如果会超出边界，重新选择方向
        TrajectoryPoint last = A.trajectory.back();
        int newX = last.x + dx[direction];
        int newY = last.y + dy[direction];
        
        while (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) {
            direction = rand() % 4;
            newX = last.x + dx[direction];
            newY = last.y + dy[direction];
            
            // 如果是第二步，还需要确保不会往回走
            if (i > 0) {
                TrajectoryPoint prev = A.trajectory[A.trajectory.size() - 2];
                TrajectoryPoint curr = A.trajectory.back();
                int lastDx = curr.x - prev.x;
                int lastDy = curr.y - prev.y;
                
                // 如果选择了相反的方向，重新选择
                if (dx[direction] == -lastDx && dy[direction] == -lastDy) {
                    continue;
                }
            }
        }
        
        // 移动A
        A.move(dx[direction], dy[direction]);
    }

    // 生成物体B的实际轨迹
    auto [bX, bY] = randomStart();
    Ship B(bX, bY);
    
    // B也只能上下左右移动一格且不会往回走
    for (int i = 0; i < 2; i++) {
        // 随机选择一个方向（0-3）
        int direction = rand() % 4;
        
        // 如果是第二步，确保不会往回走
        if (i > 0) {
            TrajectoryPoint prev = B.trajectory[B.trajectory.size() - 2];
            TrajectoryPoint curr = B.trajectory.back();
            
            // 计算上一步的方向
            int lastDx = curr.x - prev.x;
            int lastDy = curr.y - prev.y;
            
            // 避免选择相反的方向
            while ((dx[direction] == -lastDx && dy[direction] == -lastDy)) {
                direction = rand() % 4;
            }
        }
        
        // 检查移动后是否会超出边界
        TrajectoryPoint last = B.trajectory.back();
        int newX = last.x + dx[direction];
        int newY = last.y + dy[direction];
        
        // 如果会超出边界，重新选择方向
        while (newX < 0 || newX >= GRID_SIZE || newY < 0 || newY >= GRID_SIZE) {
            direction = rand() % 4;
            newX = last.x + dx[direction];
            newY = last.y + dy[direction];
            
            // 如果是第二步，还需要确保不会往回走
            if (i > 0) {
                TrajectoryPoint prev = B.trajectory[B.trajectory.size() - 2];
                TrajectoryPoint curr = B.trajectory.back();
                int lastDx = curr.x - prev.x;
                int lastDy = curr.y - prev.y;
                
                // 如果选择了相反的方向，重新选择
                if (dx[direction] == -lastDx && dy[direction] == -lastDy) {
                    continue;
                }
            }
        }
        
        // 移动B
        B.move(dx[direction], dy[direction]);
    }

    // 计算B相对于A的轨迹
    vector<TrajectoryPoint> relativeB = calculateRelativePath(A.trajectory, B.trajectory);

    // 显示游戏信息
    cout << "===== 脑力航迹游戏 =====" << endl;
    printTrajectory(A.trajectory, "船A的实际");
    printTrajectory(relativeB, "船B的相对");
    
    // 添加可视化显示（同时显示A的实际轨迹和B的相对轨迹）
    cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl;
    printCombinedGrid(A.trajectory, relativeB);
    
    // 获取玩家答案
    cout << "\n请输入B的实际航迹（格式：x1,y1 x2,y2 ...）：" << endl;
    string input;
    getline(cin, input);

    // 解析输入
    vector<TrajectoryPoint> playerAnswer;
    stringstream ss(input);
    string point;
    while (ss >> point) {
        size_t comma = point.find(',');
        int x = stoi(point.substr(0, comma));
        int y = stoi(point.substr(comma+1));
        playerAnswer.emplace_back(x, y);
    }

    // 验证答案
    bool correct = true;
    if (playerAnswer.size() != B.trajectory.size()) {
        correct = false;
    } else {
        for (size_t i = 0; i < B.trajectory.size(); i++) {
            if (playerAnswer[i].x != B.trajectory[i].x || 
                playerAnswer[i].y != B.trajectory[i].y) {
                correct = false;
                break;
            }
        }
    }

    // 显示结果
    if (correct) {
        cout << "正确！B的实际航迹：" << endl;
        printTrajectory(B.trajectory, "B的实际");
        cout << "\n最终位置图示:" << endl;
        // 使用实际轨迹显示函数
        printActualGrid(A.trajectory, B.trajectory);
    } else {
        cout << "错误！正确答案：" << endl;
        printTrajectory(B.trajectory, "B的实际");
        cout << "\n正确的最终位置图示:" << endl;
        // 使用实际轨迹显示函数
        printActualGrid(A.trajectory, B.trajectory);
        
        if (playerAnswer.size() > 0) {
            cout << "\n您的答案图示:" << endl;
            // 使用玩家答案和A的实际轨迹
            printActualGrid(A.trajectory, playerAnswer);
        }
    }

    return 0;
}
