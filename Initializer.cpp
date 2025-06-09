#include "Mental_Trajectory.h"

void AbstractInitializer::generateGridSize(){
    gridSize = 4 + 4 * steps / 7 + rand() % 2;  
}
pair<int, int> AbstractInitializer::getRandomStart(){ 
    return { rand() % gridSize, rand() % gridSize };
}
void AbstractInitializer::generateTrajectory(Ship& ship){
    // 定义可能的移动方向：上、右、下、左
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};
    
    int i = 0;  
    while(i < steps){    
        // 随机选择一个方向（0-3）
        int direction = rand() % 4;

        // 检查移动后是否会超出边界
        TrajectoryPoint last = ship.getTrajectory().back();
        int newX = last.x + dx[direction];
        int newY = last.y + dy[direction];
        if(newX < 0 || newX >= gridSize || newY < 0 || newY >= gridSize) continue; // 重新选择方向
        
        // 如果不是第一步，确保不会往回走
        if(i > 0){
            TrajectoryPoint prev = ship.getTrajectory()[ship.getTrajectory().size() - 2];
            TrajectoryPoint curr = ship.getTrajectory().back();

            // 计算上一步的方向
            int lastDx = curr.x - prev.x;
            int lastDy = curr.y - prev.y;

            // 如果选择了相反的方向，重新选择
            if(dx[direction] == -lastDx && dy[direction] == -lastDy) continue;
        }

        // 如果选择原地不动，重新选择
        if(dx[direction] == 0 && dy[direction] == 0) continue;

        // 如果没有冲突，移动船只
        ship.move(dx[direction], dy[direction]);
        i++;  // 只有成功移动后才增加计数
    }
}
void AbstractInitializer::calculateRelativePath(){
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
void AbstractInitializer::initializeAllTrajectory(){
    // 生成A的实际航迹
    auto [aX, aY] = getRandomStart();
    shipA = Ship(aX, aY);
    generateTrajectory(shipA);

    // 生成B的实际航迹
    auto [bX, bY] = getRandomStart();
    shipB = Ship(bX, bY);
    generateTrajectory(shipB);

    // 计算B相对于A的航迹
    calculateRelativePath();
}

ChallengeInitializer::ChallengeInitializer(Player& player) : AbstractInitializer(){
    setMaxDifficulty(player);
    generateSteps();
    calculateDifficulty();
    generateGridSize();
    initializeAllTrajectory();
}
void ChallengeInitializer::setMaxDifficulty(Player& player){
    if(player.getChallengeRank() == PlayerRank::BRONZE){ maxDifficulty = 1; }
    else if(player.getChallengeRank() == PlayerRank::SILVER){ maxDifficulty = 2; }
    else if(player.getChallengeRank() == PlayerRank::GOLD){ maxDifficulty = 3; }
    else if(player.getChallengeRank() == PlayerRank::PLATINUM){ maxDifficulty = 4; }
    else if(player.getChallengeRank() == PlayerRank::DIAMOND){ maxDifficulty = 5; }
    else if(player.getChallengeRank() == PlayerRank::MASTER){ maxDifficulty = 6; }
}
void ChallengeInitializer::generateSteps(){
    if(baseGameModeEngine.getBaseGameMode() == 0 || baseGameModeEngine.getBaseGameMode() == 1){
        if(maxDifficulty == 1){ steps = 5 + rand() % 2; } //6
        else if(maxDifficulty == 2){ steps = 5 + rand() % 4; } //8
        else if(maxDifficulty == 3){ steps = 5 + rand() % 6; } //10
        else if(maxDifficulty == 4){ steps = 5 + rand() % 8; } //12
        else if(maxDifficulty == 5){ steps = 5 + rand() % 10; } //14
        else if(maxDifficulty == 6){ steps = 5 + rand() % 12; }  //16
    }  
    else if(baseGameModeEngine.getBaseGameMode() == 2){
        if(maxDifficulty == 1){ steps = 4 + rand() % 2; } //5
        else if(maxDifficulty == 2){ steps = 4 + rand() % 4; } //7
        else if(maxDifficulty == 3){ steps = 4 + rand() % 6; } //9
        else if(maxDifficulty == 4){ steps = 4 + rand() % 7; }  //10
        else if(maxDifficulty == 5){ steps = 4 + rand() % 9; } //12
        else if(maxDifficulty == 6){ steps = 4 + rand() % 10; } //13
    }
}
void ChallengeInitializer::calculateDifficulty(){
    if(baseGameModeEngine.getBaseGameMode() == 0 || baseGameModeEngine.getBaseGameMode() == 1){
        difficulty = ceil((double)(steps - 4) / 2);
    }
    else if(baseGameModeEngine.getBaseGameMode() == 2){
        if(steps <= 9) { difficulty = ceil((double)(steps - 3) / 2); }
        else if(steps <= 12) { difficulty = ceil((double)(steps - 2) / 2); }
        else { difficulty = 6; }
    }
}

StageInitializer::StageInitializer(Player& player, int _stageChoice) : stageChoice(_stageChoice){
    generateSteps();
    generateGridSize();
    initializeAllTrajectory();
};
void StageInitializer::generateSteps(){
    if(stageChoice == 1){ steps = 6; }
    else if(stageChoice == 2){ steps = 8; }
    else if(stageChoice == 3){ steps = 8; }
    else if(stageChoice == 4){ steps = 10; }
    else if(stageChoice == 5){ steps = 10; }
    else if(stageChoice == 6){ steps = 11; }
    else if(stageChoice == 7){ 
        if(baseGameModeEngine.getBaseGameMode() == 0 || baseGameModeEngine.getBaseGameMode() == 1) steps = 15; 
        else if(baseGameModeEngine.getBaseGameMode() == 2) steps = 13;
    }
    else if(stageChoice == 8){
        if(baseGameModeEngine.getBaseGameMode() == 0 || baseGameModeEngine.getBaseGameMode() == 1) steps = 18;
        else if(baseGameModeEngine.getBaseGameMode() == 2) steps = 15;
    }
}

FunInitializer::FunInitializer(Player& player, int _baseGameDifficulty) : baseGameDifficulty(_baseGameDifficulty){
    generateSteps();
    generateGridSize();
    initializeAllTrajectory();
};
void FunInitializer::generateSteps(){
    if(baseGameModeEngine.getBaseGameMode() == 0 || baseGameModeEngine.getBaseGameMode() == 1){
        if(baseGameDifficulty == 1){ steps = 5 + rand() % 2; } //6
        else if(baseGameDifficulty == 2){ steps = 5 + rand() % 4; } //8
        else if(baseGameDifficulty == 3){ steps = 5 + rand() % 6; } //10
        else if(baseGameDifficulty == 4){ steps = 5 + rand() % 8; } //12
        else if(baseGameDifficulty == 5){ steps = 5 + rand() % 10; } //14
    }
    else if(baseGameModeEngine.getBaseGameMode() == 2){
        if(baseGameDifficulty == 1){ steps = 4 + rand() % 2; } //5
        else if(baseGameDifficulty == 2){ steps = 4 + rand() % 4; } //7
        else if(baseGameDifficulty == 3){ steps = 4 + rand() % 6; } //9
        else if(baseGameDifficulty == 4){ steps = 4 + rand() % 7; } //10
        else if(baseGameDifficulty == 5){ steps = 4 + rand() % 9; } //12
    }
}
