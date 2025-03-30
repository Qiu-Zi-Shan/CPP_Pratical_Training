#include<Ship.h>
#include<iostream>
using namespace std;

// 初始化位置
Ship::Ship(int startX, int startY) {
    trajectory.emplace_back(startX, startY);
}

// 添加移动轨迹
void Ship::move(int dx, int dy) {
    TrajectoryPoint last = trajectory.back();
    trajectory.emplace_back(last.x + dx, last.y + dy);
}

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