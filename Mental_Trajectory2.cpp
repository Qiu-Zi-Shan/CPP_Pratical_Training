#include "Mental_Trajectory2.h"

Ship::Ship(int startX, int startY){
    trajectory.emplace_back(startX, startY);
}

void Ship::move(int dx, int dy){
    trajectory.emplace_back(trajectory.back().x + dx, trajectory.back().y + dy);
}

const vector<TrajectoryPoint>& Ship::getTrajectory() const { return trajectory; }

GameInitializer::GameInitializer()
     : gridSize(0), steps(0), difficulty(1), shipA(0, 0), shipB(0, 0){
        generateGridSize();
        generateSteps();
        calculateDifficulty();

        //生成A的实际航迹
        auto [aX, aY] = getRandomStart();
        shipA = Ship(aX, aY);
        generateTrajectory(shipA);

        //生成B的实际航迹
        auto [bX, bY] = getRandomStart();
        shipB = Ship(bX, bY);
        generateTrajectory(shipB);
        
        // 计算B相对于A的航迹
        calculateRelativePath();
     }

pair<int, int> GameInitializer::getRandomStart(){
    return { rand() % getGridSize(), rand() % getGridSize() };
}

void GameInitializer::generateGridSize(){ 
    gridSize = 5 + rand() % 6;  // 随机生成网格大小（范围：5-10）
}

void GameInitializer::generateSteps(){
    steps = getGridSize() + rand() % (getGridSize() / 2); // 根据网格大小，随机生成航迹步数
}

void GameInitializer::generateTrajectory(Ship& ship){
    // 定义可能的移动方向：上、右、下、左
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    int i = 0;  
    while(i < getSteps()){    
        // 随机选择一个方向（0-3）
        int direction = rand() % 4;

        // 检查移动后是否会超出边界
        TrajectoryPoint last = ship.getTrajectory().back();
        int newX = last.x + dx[direction];
        int newY = last.y + dy[direction];
        if(newX < 0 || newX >= getGridSize() || newY < 0 || newY >= getGridSize()){ 
            continue;  // 重新选择方向
        }

        // 如果不是第一步，确保不会往回走
        if(i > 0){
            TrajectoryPoint prev = ship.getTrajectory()[ship.getTrajectory().size() - 2];
            TrajectoryPoint curr = ship.getTrajectory().back();
                
            // 计算上一步的方向
            int lastDx = curr.x - prev.x;
            int lastDy = curr.y - prev.y;

            // 如果选择了相反的方向，重新选择
            if(dx[direction] == -lastDx && dy[direction] == -lastDy){ 
                continue;
            }
        }
                
        // 如果选择原地不动，重新选择
        if(dx[direction] == 0 && dy[direction] == 0){ 
            continue;
        }

        // 如果没有冲突，移动船只
        ship.move(dx[direction], dy[direction]);
        i++;  // 只有成功移动后才增加计数
    }
}

void GameInitializer::calculateRelativePath(){
    // 第一个点是B的起始位置
    relativeB.emplace_back(shipB.getTrajectory()[0].x, shipB.getTrajectory()[0].y);
    
    // 计算每一步A的位移，并将其反向应用到B的位置上
     int X = shipB.getTrajectory()[0].x;
     int Y = shipB.getTrajectory()[0].y;
        
    for(size_t i = 1; i < shipB.getTrajectory().size(); i++){
        // B的实际位移
        int bDx = shipB.getTrajectory()[i].x - shipB.getTrajectory()[i-1].x;
        int bDy = shipB.getTrajectory()[i].y - shipB.getTrajectory()[i-1].y;
            
        // A的实际位移
        int aDx = shipA.getTrajectory()[i].x - shipA.getTrajectory()[i-1].x;
        int aDy = shipA.getTrajectory()[i].y - shipA.getTrajectory()[i-1].y;
            
        // 从A的视角看，B的相对位移 = B的实际位移 - A的实际位移
        int relDx = bDx - aDx;
        int relDy = bDy - aDy;
            
        // 累加相对位移
        X += relDx;
        Y += relDy;

        relativeB.emplace_back(X, Y);
    }
}

void GameInitializer::calculateDifficulty(){
    if(steps <= 6){ difficulty = 1; } 
    else if(steps <= 8){ difficulty = 2; } 
    else if(steps <= 10){ difficulty = 3; } 
    else if(steps <= 12){ difficulty = 4; }
    else{ difficulty = 5; }
}

const int& GameInitializer::getGridSize() const { return gridSize; }
const int& GameInitializer::getSteps() const { return steps; }
const int& GameInitializer::getDifficulty() const { return difficulty; }
const int& GameInitializer::getPoints() const { return difficulty; }
const Ship& GameInitializer::getShipA() const { return shipA; }
const Ship& GameInitializer::getShipB() const { return shipB; }
const vector<TrajectoryPoint>& GameInitializer::getRelativeB() const { return relativeB; }

TimeEngine::TimeEngine() : startTime(), endTime(){}

void TimeEngine::start(){
    startTime = chrono::steady_clock::now();
}

void TimeEngine::end(){
    endTime = chrono::steady_clock::now();
}

chrono::time_point<chrono::steady_clock> TimeEngine::getStartTime() const{ return startTime; }
chrono::time_point<chrono::steady_clock> TimeEngine::getEndTime() const{ return endTime; }

void TimeEngine::displayTimeCost(){
    auto timeCost = chrono::duration_cast<chrono::seconds>(getEndTime() - getStartTime());
    cout << "\n您花费了 " << timeCost.count() << " 秒完成答题。" << endl;
}

void AbstractGameMode::playGameMode(){
    vector<TrajectoryPoint> playerAnswer;
    timeStart();
    displayModeInfo();
    displayGridInfo();
    bool correct = processPlayerInput(playerAnswer);
    timeEnd();
    displayTimeCost();
    displayResult (correct, playerAnswer);
}

GameMode1::GameMode1() : initializer(), timeEngine(){}

void GameMode1::displayModeInfo() const{ 
    cout << "游戏形式: 推测B的实际航迹" << endl; 
    GameRenderer::printTrajectory(initializer.getShipA().getTrajectory(), "船A的实际");
    GameRenderer::printTrajectory(initializer.getRelativeB(), "船B的相对");
}

void GameMode1::displayGridInfo() const{ 
    cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl; 
GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getRelativeB(), initializer, true);
}

bool GameMode1::processPlayerInput(vector<TrajectoryPoint>& playerAnswer){
    // 获取玩家答案
    cout << "\n请输入B的实际航迹（格式：x1,y1 x2,y2 ...）：" << endl;
    string input;
    getline(cin, input);

    // 检查作弊码
    if(input == "999"){
        playerAnswer = initializer.getShipB().getTrajectory();
        return true;
    }

    // 解析输入
    stringstream ss(input);
    string point;
    while(ss >> point){
        size_t comma = point.find(',');
        int x = stoi(point.substr(0, comma));
        int y = stoi(point.substr(comma+1));
        playerAnswer.emplace_back(x, y);
    }

    // 验证答案
    bool correct = true;
    if(playerAnswer.size() != initializer.getShipB().getTrajectory().size()){
        correct = false;
    } 
    else{
        for(size_t i = 0; i < initializer.getShipB().getTrajectory().size(); i++){
            if(playerAnswer[i].x != initializer.getShipB().getTrajectory()[i].x || playerAnswer[i].y != initializer.getShipB().getTrajectory()[i].y){
                correct = false;
                break;
            }
        }
    }
    return correct;
}

void GameMode1::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const{
    if(correct){
        cout << "正确！B的实际航迹：" << endl;
        GameRenderer::printTrajectory(initializer.getShipB().getTrajectory(), "B的实际");
        cout << "\n最终位置图示:" << endl;
        GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getShipB().getTrajectory(), initializer, false);
        cout << "恭喜! 挑战成功！" << endl;
    } 
    else{
        cout << "错误！正确答案：" << endl;
        GameRenderer::printTrajectory(initializer.getShipB().getTrajectory(), "B的实际");
        cout << "\n正确的最终位置图示:" << endl;
        GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getShipB().getTrajectory(), initializer, false);

        cout << "\n您的答案图示:" << endl;
        // 显示A的实际轨迹和玩家的答案
        GameRenderer::printGrid(initializer.getShipA().getTrajectory(), playerAnswer, initializer, false);
        cout << "很遗憾，挑战失败！" << endl;
    }
}

void GameMode1::timeStart(){ timeEngine.start(); }
void GameMode1::timeEnd(){ timeEngine.end(); }
void GameMode1::displayTimeCost(){ timeEngine.displayTimeCost(); }

const GameInitializer& GameMode1::getInitializer() const{ return initializer; }

GameMode2::GameMode2() : initializer(), timeEngine(){}

void GameMode2::displayModeInfo() const{ 
    cout << "游戏形式: 推测B的相对航迹" << endl;
    GameRenderer::printTrajectory(initializer.getShipA().getTrajectory(), "船A的实际");
    GameRenderer::printTrajectory(initializer.getShipB().getTrajectory(), "船B的实际"); 
}

void GameMode2::displayGridInfo() const{ 
    cout << "\n参照物A的实际位置和物体B的实际航迹:" << endl;
    GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getShipB().getTrajectory(), initializer, false);
}

bool GameMode2::processPlayerInput(vector<TrajectoryPoint>& playerAnswer){
    // 获取玩家答案
    cout << "\n请输入B的相对航迹（格式：x1,y1 x2,y2 ...）：" << endl;
    string input;
    getline(cin, input);

    // 检查作弊码
    if(input == "999"){
        playerAnswer = initializer.getRelativeB();
        return true;
    }

    // 原有的输入处理逻辑
    stringstream ss(input);
    string point;
    while(ss >> point){
        size_t comma = point.find(',');
        int x = stoi(point.substr(0, comma));
        int y = stoi(point.substr(comma+1));
        playerAnswer.emplace_back(x, y);
    }

    // 验证答案
    if(playerAnswer.size() != initializer.getRelativeB().size()){
        return false;
    } 
    else{
        for(size_t i = 0; i < initializer.getRelativeB().size(); i++){
            if(playerAnswer[i].x != initializer.getRelativeB()[i].x || playerAnswer[i].y != initializer.getRelativeB()[i].y){
                return false;
                break;
            }
        }
    }
    return true;
}

void GameMode2::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const{
    if(correct){
        cout << "正确！B的相对航迹：" << endl;
        GameRenderer::printTrajectory(initializer.getRelativeB(), "B的相对");
        cout << "\n最终位置图示:" << endl;
        // 显示A的实际轨迹和B的相对轨迹
        GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getRelativeB(), initializer, true);
        cout << "恭喜! 挑战成功！" << endl;
    } 
    else {
        cout << "错误！正确答案：" << endl;
        GameRenderer::printTrajectory(initializer.getRelativeB(), "B的相对");
        cout << "\n正确的最终位置图示:" << endl;
        // 显示A的实际轨迹和B的相对轨迹
        GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getRelativeB(), initializer, true);
        
        cout << "\n您的答案图示:" << endl;
        // 显示A的实际轨迹和玩家的答案
        GameRenderer::printGrid(initializer.getShipA().getTrajectory(), playerAnswer, initializer, true);
        cout << "很遗憾，挑战失败！" << endl;
    }
}

void GameMode2::timeStart(){ timeEngine.start(); }
void GameMode2::timeEnd(){ timeEngine.end(); }
void GameMode2::displayTimeCost(){ timeEngine.displayTimeCost(); }

const GameInitializer& GameMode2::getInitializer() const{ return initializer; }

GameMode3::GameMode3() : initializer(), timeEngine(), isRelative(false){}

void GameMode3::setIsRelative(){ isRelative = rand() % 2; }

bool GameMode3::getIsRelative() const{ return isRelative; }

void GameMode3::displayModeInfo() const{ cout << "游戏形式: 逐步推测B的航迹" << endl; }

void GameMode3::displayGridInfo() const{
    if(!getIsRelative()){ cout << "\n参照物A的实际位置和物体B的相对航迹:" << endl; } 
    else{ cout << "\n参照物A的实际位置和物体B的实际航迹:" << endl; }
}

bool GameMode3::processPlayerInput(vector<TrajectoryPoint>& playerAnswer){
    cout << "\n请输入B的" << (getIsRelative() ? "相对" : "实际") << "航迹（格式：x,y）：" << endl;
    string input;
    getline(cin, input);

    // 检查作弊码
    if(input == "999"){
        if(getIsRelative()){
            playerAnswer.push_back(initializer.getRelativeB()[playerAnswer.size()]);
            return true;
        }
        else{
            playerAnswer.push_back(initializer.getShipB().getTrajectory()[playerAnswer.size()]);
            return true;
        }
    }

    size_t comma = input.find(',');
    int x = stoi(input.substr(0, comma));
    int y = stoi(input.substr(comma+1));
    playerAnswer.push_back(TrajectoryPoint(x, y));
    
    if(getIsRelative()){
        return(playerAnswer.back().x == initializer.getRelativeB()[playerAnswer.size()-1].x && 
        playerAnswer.back().y == initializer.getRelativeB()[playerAnswer.size()-1].y);
    }
    else{
        return(playerAnswer.back().x == initializer.getShipB().getTrajectory()[playerAnswer.size()-1].x &&
        playerAnswer.back().y == initializer.getShipB().getTrajectory()[playerAnswer.size()-1].y);
    }
    return false;
}

void GameMode3::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer) const{
    if(correct){
        cout << "正确！继续下一步。" << endl;
    } 
    else{
        cout << "错误！正确的" << (getIsRelative() ? "相对" : "实际") << "位置应该是：";
        if(getIsRelative()){
            cout << initializer.getRelativeB()[playerAnswer.size()-1].x << "," << initializer.getRelativeB()[playerAnswer.size()-1].y << endl;
        } 
        else{
            cout << initializer.getShipB().getTrajectory()[playerAnswer.size()-1].x << "," << initializer.getShipB().getTrajectory()[playerAnswer.size()-1].y << endl;
        }
        cout << "很遗憾，挑战失败！" << endl;
    }
}

void GameMode3::playGameMode(){
    // 开始计时
    timeStart();

    bool correct = true;
    vector<TrajectoryPoint> playerAnswer;
    for(size_t step = 1; step < initializer.getShipA().getTrajectory().size() && correct; ++step){
        // 随机决定这一步是猜实际航迹还是相对航迹
        setIsRelative();

        // 显示A的当前轨迹
        vector<TrajectoryPoint> currentA(initializer.getShipA().getTrajectory().begin(), initializer.getShipA().getTrajectory().begin() + step + 1);
        GameRenderer::printTrajectory(currentA, "船A的实际");

        // 准备当前步骤的轨迹
        vector<TrajectoryPoint> currentB(initializer.getShipB().getTrajectory().begin(), initializer.getShipB().getTrajectory().begin() + step + 1);
        vector<TrajectoryPoint> currentRelativeB(initializer.getRelativeB().begin(), initializer.getRelativeB().begin() + step + 1);

        if(!getIsRelative()){
            // 如果要猜B的实际航迹，显示完整的相对航迹，但实际航迹少显示一步
            GameRenderer::printTrajectory(currentRelativeB, "船B的相对");
            vector<TrajectoryPoint> prevB(initializer.getShipB().getTrajectory().begin(), initializer.getShipB().getTrajectory().begin() + step);
            GameRenderer::printTrajectory(prevB, "船B的实际");
            displayGridInfo();
            GameRenderer::printGrid(currentA, currentRelativeB, initializer, true);
        } 
        else{
            // 如果要猜B的相对航迹，显示完整的实际航迹，但相对航迹少显示一步
            GameRenderer::printTrajectory(currentB, "船B的实际");
            vector<TrajectoryPoint> prevRelativeB(initializer.getRelativeB().begin(), initializer.getRelativeB().begin() + step);
            GameRenderer::printTrajectory(prevRelativeB, "船B的相对");
            displayGridInfo();
            GameRenderer::printGrid(currentA, currentB, initializer, false);
        }
        // 处理玩家输入
        correct = processPlayerInput(playerAnswer);

        // 显示当前步骤结果
        displayResult(correct, playerAnswer);
    }
    if(correct){
        cout << "恭喜！挑战成功！" << endl;
    }
    //结束计时并显示用时
    timeEnd();
    displayTimeCost();
}

void GameMode3::timeStart(){ timeEngine.start(); }
void GameMode3::timeEnd(){ timeEngine.end(); }
void GameMode3::displayTimeCost(){ timeEngine.displayTimeCost(); }

const GameInitializer& GameMode3::getInitializer() const{ return initializer; }

void GameRenderer::printTrajectory(const vector<TrajectoryPoint>& trajectory, const string& name){
    cout << name << "轨迹：";
    for(const TrajectoryPoint& p : trajectory){
        cout << "(" << p.x << ", " << p.y << ") ";
    }
    cout << endl;
}

void GameRenderer::printGrid(const vector<TrajectoryPoint>& actualshipA, const vector<TrajectoryPoint>& shipB, const GameInitializer& initializer,bool isRelative){
    int gridSize = initializer.getGridSize(); 
    char grid[gridSize][gridSize]; 
    
    // 初始化网格
    for(int y = 0; y < gridSize; y++){
        for(int x = 0; x < gridSize; x++){
            grid[y][x] = '.';
        }
    }
    
    // 标记A的航迹
    for(int i = 0; i < actualshipA.size(); i++){
        int x = actualshipA[i].x;
        int y = actualshipA[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            grid[y][x] = 'A';
        }
    }
    
    // 标记B航迹
    for(int i = 0; i < shipB.size(); i++){
        int x = shipB[i].x;
        int y = shipB[i].y;
        if(x >= 0 && x < gridSize && y >= 0 && y < gridSize){
            // 如果与A重叠，用'*'表示
            if(grid[y][x] == 'A'){
                grid[y][x] = '*';
            } 
            else{
                if(isRelative){ grid[y][x] = 'R'; }
                else { grid[y][x] = 'B';}
            }
        }
    }
    
    // 打印网格
    cout << "  ";
    for(int x = 0; x < gridSize; x++){
        cout << x << " ";
    }
    cout << endl;
    
    for(int y = 0; y < gridSize; y++){
        cout << y << " ";
        for(int x = 0; x < gridSize; x++){
            cout << grid[y][x] << " ";
        }
        cout << endl;
    }
    cout << "图例: A=船A的航迹, B=船B的航迹, R=船B的相对航迹, *=A和B或R重叠, .=空格" << endl;
}

GameMode::GameMode(){
    setCurrentGameMode();
}

GameMode::~GameMode(){
    delete currentGameMode;
}

void GameMode::setCurrentGameMode(){
    int gameMode = rand() % 3; // 随机选择游戏模式
    switch(gameMode){
        case 0:
            currentGameMode = new GameMode1();
            break;
        case 1:
            currentGameMode = new GameMode2();
            break;
        case 2:
            currentGameMode = new GameMode3();
            break;
    }
}

AbstractGameMode* GameMode::getCurrentMode(){ return currentGameMode; }

void GameMode::beginBaseGameMode() {
    currentGameMode -> playGameMode();
}

BaseGameRun::BaseGameRun() : gameMode(){}

void BaseGameRun::run(){
    cout << "网格大小: " << gameMode.getCurrentMode() -> getInitializer().getGridSize() << "x" << gameMode.getCurrentMode() -> getInitializer().getGridSize() << endl;
    cout << "航迹步数: " << gameMode.getCurrentMode() -> getInitializer().getSteps() << endl;
    cout << "难度级别: " << gameMode.getCurrentMode() -> getInitializer().getDifficulty() << "级 (1级最简单)" << endl;

    vector<TrajectoryPoint> playerAnswer;
    gameMode.beginBaseGameMode();
}










