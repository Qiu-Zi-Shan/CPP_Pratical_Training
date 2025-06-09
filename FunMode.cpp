#include "Mental_Trajectory.h"

HexCell::HexCell() : difficulty(1), isFlag(false), isStartPoint(false), row(0), col(0), isMapCell(false), hasBeenChoosen(false){}
HexCell::HexCell(int r, int c) 
    : difficulty(1), isFlag(false), isStartPoint(false), row(r), col(c), isMapCell(false), hasBeenChoosen(false){}
void HexCell::setDifficulty(int level){ difficulty = max(1, min(4, level)); } 
int HexCell::getDifficulty() const{ return difficulty; }
bool HexCell::isNeighborWith(HexCell* cell) const{
    return find(neighbors.begin(), neighbors.end(), cell) != neighbors.end();
}
void HexCell::addNeighbor(HexCell* cell){
    if(cell != this && !isNeighborWith(cell)){
        neighbors.push_back(cell);
        cell->addNeighbor(this); // 双向连接
    }
}
vector<HexCell*> HexCell::getNeighbors() const{ return neighbors; }
void HexCell::setFlag(bool flag){ isFlag = flag; }
bool HexCell::getIsFlag() const{ return isFlag; }
void HexCell::setStartPoint(bool startPoint){ isStartPoint = startPoint; }
bool HexCell::getIsStartPoint() const{ return isStartPoint; }
pair<int, int> HexCell::getPosition() const{ return {row, col}; }
void HexCell::setIsMapCell(bool isMap) { isMapCell = isMap; }
bool HexCell::getIsMapCell() const {  return isMapCell; }
void HexCell::setHasBeenChoosen(bool chosen){ hasBeenChoosen = chosen; }
bool HexCell::getHasBeenChoosen() const{ return hasBeenChoosen; }

HexMap::HexMap(int gameDifficulty) : flagCell(nullptr){
    if(gameDifficulty == 1) size = 13;
    else if(gameDifficulty == 2) size = 15;
    else if(gameDifficulty == 3) size = 17;
    else size = 13;
    // 创建地图网格
    cells.resize(size);
    for(int r = 0; r < size; r++){
        cells[r].resize(size);
        for(int c = 0; c < size; c++){
            cells[r][c] = HexCell(r, c);
        }
    }  
    // 完善每个单元格的邻居
    const pair<int, int> evenOffsets[] = {
        {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {0, -1}
    };
    const pair<int, int> oddOffsets[] = {
        {-1, -1}, {-1, 0}, {0, 1}, {1, 0}, {1, -1}, {0, -1}
    };
    for(int r = 0; r < size; r++){
        auto& offsets = (r % 2 == 0) ? evenOffsets : oddOffsets;
        for(int c = 0; c < size; c++){
            for(int k = 0; k < 6; k++){
                int nr = r + offsets[k].first;
                int nc = c + offsets[k].second;
                if(nr >= 0 && nr < size && nc >= 0 && nc < size){
                    cells[r][c].addNeighbor(&cells[nr][nc]);
                }
            }
        }
    }
    // 设置旗帜（中心位置）
    setFlag(); 
    // 生成地图
    generateMap();
    // 设置难度值
    setBaseDifficulties();
    // 生成起点
    generateStartPoints();
}
void HexMap::generateMap(){
    vector<vector<HexCell*>> outNeighbors((size + 1) / 2, vector<HexCell*>());
    // 设置旗帜为地图单元格
    flagCell->setIsMapCell(true);
    outNeighbors[0].push_back(flagCell);

    // 以旗帜为中心，向外扩展地图
    // 由内向外分解地图
    auto [flagR, flagC] = flagCell->getPosition();
    // 随机数生成器
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    
    for(int i = 0; i < (size + 1) / 2 - 1; i++){
        for(auto cell : outNeighbors[i]){
            for(auto neighbor : cell->getNeighbors()){
                if(find(outNeighbors[i + 1].begin(), outNeighbors[i + 1].end(), neighbor) == outNeighbors[i + 1].end() && find(outNeighbors[i].begin(), outNeighbors[i].end(), neighbor) == outNeighbors[i].end()){
                    outNeighbors[i + 1].push_back(neighbor);
                }
            }
            // 随机决定是否设置为地图单元格(外层设置为地图单元格概率变小)
            if(dis(gen) < max(0.5, 1 - pow(1.6, i) / 100)){
                cell -> setIsMapCell(true);
            }
        }
    }
}
void HexMap::setFlag(){
    // 在地图中心设置旗帜
    int centerR = ceil(size / 2);
    int centerC = ceil(size / 2);
    flagCell = &cells[centerR][centerC];
    flagCell->setFlag(true);
}
void HexMap::setBaseDifficulties(){
    // 设置旗帜的难度为最高
    getFlagCell()->setDifficulty(5);
    // 其余根据到旗帜的距离设置难度
    for(int r = 0; r < size; r++){
        for(int c = 0; c < size; c++){
            HexCell* cell = &cells[r][c];
            if(cell != flagCell){
                // 计算到旗帜的距离
                int dist = calculateDistance(cell, flagCell, size);
                // 根据距离设置难度（越靠近旗帜，难度越高）
                int difficulty = 4.0 - 3.0 * (static_cast<double>(dist) / ((size + 1) / 2));
                cell->setDifficulty(difficulty);
            }
        }
    }
}
void HexMap::generateStartPoints(){
    // 清空之前的起点
    startPoints.clear();  
    // 寻找适合作为起点的单元格候选（距离相似且均匀分布）
    vector<HexCell*> candidateCells; 
    // 计算到旗帜的最大距离
    int maxDist = 0;
    for(int r = 0; r < size; r++){
        for(int c = 0; c < size; c++){
            HexCell* cell = &cells[r][c];
            int dist = calculateDistance(cell, flagCell, size);
            maxDist = max(maxDist, dist);
        }
    }
    // 继续收集候选单元格直到达到5个或已尝试所有可能的距离
    for(int i = 0; startPoints.size() < 5 && i <= maxDist; i++){  
        // 收集所有距离为 maxDist-i 的单元格
        for(int r = 0; r < size; r++){
            for(int c = 0; c < size; c++){
                HexCell* cell = &cells[r][c];
                // 只考虑地图单元格
                if(cell->getIsMapCell() && !cell->getIsFlag()){
                    int dist = calculateDistance(cell, flagCell, size);
                    if(dist == maxDist - i){
                        candidateCells.push_back(cell);
                    }
                }
            }
        }
        
        // 如果当前距离层有候选单元格，随机打乱并添加
        if(!candidateCells.empty()){
            // 随机打乱当前距离的候选单元格，确保均匀分布
            random_device rd;
            mt19937 gen(rd());
            shuffle(candidateCells.begin(), candidateCells.end(), gen);
            
            // 添加到主候选列表，但不超过总共5个
            int numToAdd = min(static_cast<int>(candidateCells.size()), 
                              5 - static_cast<int>( startPoints.size()));
            startPoints.insert(startPoints.end(), 
                                candidateCells.begin(),candidateCells.begin() + numToAdd);
            candidateCells.clear();
        }
    }
  
    for(int i = 0; i < 5; i++){
        startPoints[i]->setStartPoint(true);
    }
}
int HexMap::calculateDistance(HexCell* cell, HexCell* flagCell, int mapSize){   
    map<HexCell*, int> distance;
    // 如果是旗帜本身，距离为0
    if(cell == flagCell){ return 0; }

    // 如果不是地图单元格，返回一个很大的值表示不可达
    if(!cell->getIsMapCell()){ return mapSize * mapSize; }
    
    // 使用广度优先搜索寻找最短路径
    queue<HexCell*> bfsQueue;
    
    // 初始化
    bfsQueue.push(flagCell);
    distance[flagCell] = 0;
    
    // BFS搜索
    while(!bfsQueue.empty()){
        HexCell* current = bfsQueue.front();
        bfsQueue.pop();
        
        // 如果找到目标单元格，返回距离
        if(current == cell){ return distance[current]; }
        
        // 检查所有邻居
        for(HexCell* neighbor : current->getNeighbors()){
            // 只考虑是地图单元格且尚未访问过的邻居
            if(neighbor->getIsMapCell() && distance.find(neighbor) == distance.end()){
                distance[neighbor] = distance[current] + 1;
                bfsQueue.push(neighbor);
            }
        }
    }
    // 如果无法到达目标单元格，返回一个很大的值
    return mapSize * mapSize;
}
void HexMap::printMap() const{ 
    for(int r = 0; r < size; r++){
        if(r % 2 == 0){
            cout << "  ";
            for(int c = 0; c < size; c++){
                const HexCell& cell = cells[r][c];
                if(cell.getIsMapCell()){ 
                    if(!cell.getIsFlag() && !cell.getIsStartPoint()) { cout << cell.getDifficulty() << "   "; }
                    else if(cell.getIsFlag()) { cout << "F" << "   "; }
                    else if(cell.getIsStartPoint()){ 
                        for(int i = 0; i < startPoints.size(); i++){
                            if(startPoints[i] == &cell) { cout << static_cast<char>(65 + i) << "   "; break; }
                        }
                    }
                }
                else{ cout << "#   "; }
            }
            cout << endl;
        }
        else{
            for(int c = 0; c < size; c++){
                const HexCell& cell = cells[r][c];
                if(cell.getIsMapCell()){ 
                    if(!cell.getIsFlag() && !cell.getIsStartPoint()) { cout << cell.getDifficulty() << "   "; }
                    else if(cell.getIsFlag()) { cout << "F" << "   "; }
                    else if(cell.getIsStartPoint()) { 
                        for(int i = 0; i < startPoints.size(); i++){
                            if(startPoints[i] == &cell) { cout << static_cast<char>(65 + i) << "   "; break; }
                        }
                    }
                }
                else{ cout << "#   "; }
            }
            cout << endl;
        }
    }
}
HexCell* HexMap::getFlagCell() const { return flagCell; }
vector<HexCell*> HexMap::getStartCells() const { return startPoints; }
HexCell* HexMap::getCellAt(int row, int col){
    if(row >= 0 && row < size && col >= 0 && col < size){ return &cells[row][col]; }
    return nullptr;
}
int HexMap::getSize() const { return size; }

// FunModeEngine 实现
FunModeEngine::FunModeEngine(Player& player, int gameDifficulty)
    : gameMap(gameDifficulty), human(player), ai(gameDifficulty), isHumanTurn(true), gameOver(false), firstRound(true), gameDifficulty(gameDifficulty){
    // 打印地图
    gameMap.printMap();
}
bool FunModeEngine::startFunMode(){
    // 选择起点
    chooseStartPoints();
    
    // 游戏主循环
    while(!isGameOver()){
        playRound();
    }

    if(human.hasWon()) return true; 
    else{
        cout << "游戏结束，玩家失败。" << endl;
        return false;
    }
}
void FunModeEngine::chooseStartPoints(){
    cout << "\n==== 选择起点阶段 ====" << endl;
    
    // 在第一轮中，获胜者（或随机决定）先选择起点
    if(firstRound){
        // 随机决定谁先选
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, 1);
        isHumanTurn = dis(gen) == 1;
        
        cout << (isHumanTurn ? "玩家" : "AI") << "先选择起点" << endl;
    }
    
    // 先选择的玩家选择起点
    if(isHumanTurn){
        HexCell* humanStartCell = human.chooseStartCell(gameMap.getStartCells());
        human.setStartCell(humanStartCell);
        
        // AI选择起点
        HexCell* aiStartCell = ai.chooseStartCell(gameMap.getStartCells(), gameMap.getFlagCell(), gameMap.getSize());
        ai.setStartCell(aiStartCell);
    } 
    else{
        HexCell* aiStartCell = ai.chooseStartCell(gameMap.getStartCells(), gameMap.getFlagCell(), gameMap.getSize());
        ai.setStartCell(aiStartCell);
        
        // 玩家选择起点
        HexCell* humanStartCell = human.chooseStartCell(gameMap.getStartCells());
        human.setStartCell(humanStartCell);
    }
    
    firstRound = false;
    displayGameState();
}
void FunModeEngine::playRound(){
    cout << "\n==== " << (isHumanTurn ? "玩家" : "AI") << "回合 ====" << endl;
    
    // 当前玩家
    FunModePlayer* currentPlayer = isHumanTurn ? static_cast<FunModePlayer*>(&human) : static_cast<FunModePlayer*>(&ai);
    
    // 处理当前玩家的回合
    bool success = processTurn(currentPlayer);
    
    // 检查是否有人获胜
    if(currentPlayer->hasWon()){
        gameOver = true;
        cout << "\n=== " << (isHumanTurn ? "玩家" : "AI") << "获胜! ===" << endl;
        return;
    }
    
    // 交换回合
    isHumanTurn = !isHumanTurn;
}
bool FunModeEngine::processTurn(FunModePlayer* currentPlayer){
    // 获取当前位置
    HexCell* currentCell = currentPlayer->getCurrentCell();

    // 选择下一步移动
    HexCell* targetCell;
    if(isHumanTurn){ targetCell = human.chooseNextMove(); } 
    else { targetCell = ai.chooseNextMove(); }
    
    // 执行游戏挑战
    int baseGameDifficulty = targetCell->getDifficulty();
    bool challengeSuccess;
    
    if(isHumanTurn){ challengeSuccess = human.playGame(baseGameDifficulty); } 
    else{ challengeSuccess = ai.playGame(baseGameDifficulty); }
    
    // 根据挑战结果移动
    if(challengeSuccess){
        cout << "#挑战成功! 向前移动" << endl;
        currentPlayer->moveToCell(targetCell);
        if(currentPlayer->getCurrentCell()->getIsFlag()){
            cout << "【" << (isHumanTurn ? "玩家" : "AI") << "】已到达旗帜位置！" << endl;
        }
    } 
    else{
        cout << "\n#挑战失败! 原地不动" << endl;
    }
    
    // 显示移动后的地图
    displayGameState();
    
    return challengeSuccess;
}

bool FunModeEngine::isGameOver() const { return gameOver; }
FunModePlayer* FunModeEngine::getWinner() const{
    if(gameOver){
        if(human.hasWon()) return const_cast<FunModePlayer*>(static_cast<const FunModePlayer*>(&human));
        else if(ai.hasWon()) return const_cast<FunModePlayer*>(static_cast<const FunModePlayer*>(&ai));
    }
    return nullptr;
}

void FunModeEngine::displayGameState(){
    cout << "\n当前游戏状态:" << endl;
    
    // 创建一个临时地图用于显示
    vector<vector<char>> displayMap(gameMap.getSize(), vector<char>(gameMap.getSize(), ' '));

    // 填充地图基本信息 (难度、边界等)
    for(int r = 0; r < gameMap.getSize(); r++){
        for(int c = 0; c < gameMap.getSize(); c++){
            HexCell* cell = gameMap.getCellAt(r, c);
            if(cell->getIsMapCell()){
                // 显示难度，但会被其他符号覆盖
                displayMap[r][c] = '0' + cell->getDifficulty();
            } 
            else{
                displayMap[r][c] = '#';
            }
        }
    }

    // 用一个独立的 map 来记录每个格子的访问状态
    map<HexCell*, char> trail;

    // 记录 AI 的轨迹
    for(auto* cell : ai.getMoveHistory()){
        trail[cell] = 'X';
    }

    // 记录玩家的轨迹，处理重叠情况
    for(auto* cell : human.getMoveHistory()){
        if(trail.count(cell)){
            trail[cell] = '*'; // 轨迹重叠
        } 
        else{
            trail[cell] = 'O'; // 只有玩家走过
        }
    }
    
    // 将轨迹应用到显示地图上
    for(auto const& [cell, symbol] : trail){
        auto [r, c] = cell->getPosition();
        displayMap[r][c] = symbol;
    }

    // 标记特殊点 (旗帜)，确保它们不会被轨迹覆盖
    auto [r, c] = gameMap.getFlagCell()->getPosition();
    displayMap[r][c] = 'F';

    if(human.getCurrentCell()){
        if(!human.getCurrentCell()->getIsFlag()) { auto [r, c] = human.getCurrentCell()->getPosition(); displayMap[r][c] = 'H'; } 
    }

    if(ai.getCurrentCell()){
        if(!ai.getCurrentCell()->getIsFlag()){ 
            auto [r, c] = ai.getCurrentCell()->getPosition(); 
            if(displayMap[r][c] == 'H') { displayMap[r][c] = '*'; }
            else{ displayMap[r][c] = 'I'; }
        }
    }

    // 打印最终地图
    for(int r = 0; r < gameMap.getSize(); r++){
        // 为奇数行添加缩进，以模拟六边形布局
        if(r % 2 == 0){
            cout << "  ";
        }
        for(int c = 0; c < gameMap.getSize(); c++){
            cout << displayMap[r][c] << "   ";
        }
        cout << endl;
    }

    // 图例
    cout << "图例: F=旗帜, A~E=起点, 1-4=难度" << endl;
    cout << "      H=玩家当前位置, A=AI当前位置" << endl;
    cout << "      O=玩家轨迹, X=AI轨迹, *=重叠轨迹" << endl;
}






