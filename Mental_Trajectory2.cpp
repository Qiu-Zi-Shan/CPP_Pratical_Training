#include "Mental_Trajectory2.h"

Ship::Ship(int startX, int startY){
    trajectory.emplace_back(startX, startY);
}

void Ship::move(int dx, int dy){
    trajectory.emplace_back(trajectory.back().x + dx, trajectory.back().y + dy);
}

const vector<TrajectoryPoint>& Ship::getTrajectory() const { return trajectory; }

Player::Player() : challengeScore(0), challengeGames(0), challengeSuccessGames(0), currentStage(0){}
Player::Player(const string& name, const string& pwd) : username(name), password(pwd),  
                challengeScore(0), challengeGames(0), challengeSuccessGames(0), currentStage(0){}
                
bool Player::checkPassword(const string& pwd) const{ return pwd == password; }
string Player::getUsername() const{ return username; }

void Player::setChallengeScore(int newScore) { challengeScore = newScore; }
void Player::addChallengeScore(int points) { challengeScore += points; }
void Player::reduceChallengeScore(int points) { challengeScore -= points; }
void Player::setTotalChallengeGames(int totalGames) { challengeGames = totalGames; }
void Player::setSuccessChallengeGames(int successGames) { challengeSuccessGames = successGames; }
void Player::addChallengeGame(bool success) { 
    challengeGames++;
    if(success) challengeSuccessGames++;
}
void Player::calculateRank(){
    if(challengeScore < 50) rank = PlayerRank::BRONZE;
    else if(challengeScore < 150) rank = PlayerRank::SILVER;
    else if(challengeScore < 300) rank = PlayerRank::GOLD;
    else if(challengeScore < 500) rank = PlayerRank::PLATINUM;
    else if(challengeScore < 800) rank = PlayerRank::DIAMOND;
    else rank = PlayerRank::MASTER;
}

int Player::getChallengeScore() const{ return challengeScore; }
int Player::getTotalChallengeGames() const{ return challengeGames; }
int Player::getSuccessChallengeGames() const{ return challengeSuccessGames; }
double Player::getChallengeSuccessRate() const{ 
    return challengeGames == 0 ? 0 : static_cast<double>(challengeSuccessGames) / challengeGames;
}
PlayerRank Player::getChallengeRank() const { return rank; }
string Player::getChallengeRankName() const {
    switch(getChallengeRank()){
        case PlayerRank::BRONZE: return "青铜";
        case PlayerRank::SILVER: return "白银";
        case PlayerRank::GOLD: return "黄金";
        case PlayerRank::PLATINUM: return "铂金";
        case PlayerRank::DIAMOND: return "钻石";
        case PlayerRank::MASTER: return "大师";
        default: return "未知";
    }
}

void Player::setCurrentStage(int stage) { currentStage = stage; }
int Player::getCurrentStage() const { return currentStage; }

void Player::setStageTime(int stage, int timeSeconds){
    // 如果没有该关卡记录或新时间更短，则更新
    if(!hasStageTimeRecord(stage) || timeSeconds < stageTimeRecords[stage]){
        stageTimeRecords[stage] = timeSeconds;
    }
}
int Player::getStageTime(int stage) const{
    auto it = stageTimeRecords.find(stage);
    if(it != stageTimeRecords.end()){
        return it->second;
    }
    return 0; // 返回0表示没有该关卡的记录
}

bool Player::hasStageTimeRecord(int stage) const{
    return stageTimeRecords.find(stage) != stageTimeRecords.end();
}

TimeEngine::TimeEngine() : startTime(), endTime(), pausedDuration(chrono::seconds(0)) {}

void TimeEngine::start(){
    startTime = chrono::steady_clock::now();
}
void TimeEngine::end(){
    endTime = chrono::steady_clock::now();
}
void TimeEngine::pause(){
    pauseStartTime = chrono::steady_clock::now();
}
void TimeEngine::resume(){
    auto pauseEndTime = chrono::steady_clock::now();
    pausedDuration += (pauseEndTime - pauseStartTime);
}

chrono::time_point<chrono::steady_clock> TimeEngine::getStartTime() const{ return startTime; }
chrono::time_point<chrono::steady_clock> TimeEngine::getEndTime() const{ return endTime; }
chrono::duration<double> TimeEngine::getPausedDuration() const{ return pausedDuration; }
chrono::seconds TimeEngine::getTimeCost(){ 
    return chrono::duration_cast<chrono::seconds>(getEndTime() - getStartTime() - getPausedDuration()); 
}

void TimeEngine::displayTimeCost(){
    cout << "\n您花费了 " << getTimeCost().count() << " 秒完成答题。" << endl;
}

bool AbstractBaseGameMode::playBaseGameMode(GameInitializer& initializer){
    vector<TrajectoryPoint> playerAnswer;
    timeStart();
    displayBaseModeInfo(initializer);
    displayGridInfo(initializer);
    cout << "\n游戏提示：输入 'p/P' 可随时暂停/继续游戏" << endl; 
    
    // 处理玩家输入
    bool correct = processPlayerInput(playerAnswer, initializer);
    
    timeEnd();
    displayTimeCost();
    displayResult(correct, playerAnswer, initializer);
    return correct;
}
void AbstractBaseGameMode::setIsStageMode(bool isStageMode){
    this->isStageMode = isStageMode;
}

BaseGameMode1::BaseGameMode1() : timeEngine(){}

void BaseGameMode1::displayBaseModeInfo(GameInitializer& initializer) const{ 
    cout << "游戏形式: 推测B的实际航迹" << endl; 
    GameRenderer::printTrajectory(initializer.getShipA().getTrajectory(), "A的实际");
    GameRenderer::printTrajectory(initializer.getRelativeB(), "B的相对");
}
void BaseGameMode1::displayGridInfo(GameInitializer& initializer) const{ 
    cout << "\n参照物A的实际位置和B的相对航迹:" << endl; 
    GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getRelativeB(), initializer, true);
}

bool BaseGameMode1::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer){
    playerAnswer.clear(); // 清空之前的答案
    
    cout << "\n请逐步输入B的实际航迹，共" << initializer.getShipB().getTrajectory().size() << "步" << endl;
    
    // 逐步获取玩家输入
    for(size_t i = 0; i < initializer.getShipB().getTrajectory().size(); i++){
        cout << "第" << (i+1) << "步 (格式: x,y): ";
        
        // 如果是闯关模式，显示剩余时间
        if(isStageMode){
            auto currentTime = chrono::steady_clock::now();
            auto elapsedTime = chrono::duration_cast<chrono::seconds>(
                currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
            int remainingTime = 150 - elapsedTime.count();
            if(remainingTime <= 0){
                cout << "\n时间已用尽！" << endl;
                return false;
            }
            cout << "[剩余时间: " << remainingTime << "秒] ";
        }
        
        string input;
        getline(cin, input);
 
        // 检查是否有暂停请求
        if(input == "p" || input == "P"){
            pauseGame();
            cout << "游戏已暂停，请输入 'p/P' 继续游戏" << endl;
            // 循环等待直到接收到继续游戏的命令
            while(true){
                getline(cin, input);
                if(input == "p" || input == "P"){
                    resumeGame();
                    cout << "游戏已继续！" << endl;
                    break;
                }
                cout << "游戏仍处于暂停状态，请输入 'p/P' 继续游戏" << endl;
            }      
            i--;
            continue;
        }
        
        // 如果是闯关模式，检查是否超时
        if(isStageMode && isTimeExceeded()){
            cout << "\n时间已用尽！" << endl;
            return false;
        }

        // 检查作弊码
        if(input == "999"){
            playerAnswer = initializer.getShipB().getTrajectory();
            break;
        }       
        
        // 解析输入
        size_t comma = input.find(',');
        if(comma != string::npos){
            try{
                int x = stoi(input.substr(0, comma));
                int y = stoi(input.substr(comma+1));
                playerAnswer.emplace_back(x, y);
            } 
            catch(const exception& e){
                cout << "输入格式错误，请重新输入此步骤" << endl;
                i--;
                continue;
            }
        } 
        else{
            cout << "输入格式错误，请重新输入此步骤" << endl;
            i--;
        }
    } 
    
    // 验证答案
    for(size_t i = 0; i < initializer.getShipB().getTrajectory().size(); i++){
        if(playerAnswer[i].x != initializer.getShipB().getTrajectory()[i].x || 
           playerAnswer[i].y != initializer.getShipB().getTrajectory()[i].y){
            return false;
        }
    }
    return true;
}
void BaseGameMode1::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const{
    if(correct){
        cout << "正确！" << endl;
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

void BaseGameMode1::timeStart(){ timeEngine.start(); }
void BaseGameMode1::timeEnd(){ timeEngine.end(); }
chrono::seconds BaseGameMode1::getTimeCost(){ return timeEngine.getTimeCost(); }
void BaseGameMode1::displayTimeCost(){ timeEngine.displayTimeCost(); }

void BaseGameMode1::pauseGame(){ timeEngine.pause(); }
void BaseGameMode1::resumeGame(){ timeEngine.resume(); }

bool BaseGameMode1::isTimeExceeded(int timeLimit) const {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(
        currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
    return elapsedTime.count() > timeLimit;
}

BaseGameMode2::BaseGameMode2() : timeEngine(){}

void BaseGameMode2::displayBaseModeInfo(GameInitializer& initializer) const{ 
    cout << "游戏形式: 推测B的相对航迹" << endl;
    GameRenderer::printTrajectory(initializer.getShipA().getTrajectory(), "A的实际");
    GameRenderer::printTrajectory(initializer.getShipB().getTrajectory(), "B的实际"); 
}
void BaseGameMode2::displayGridInfo(GameInitializer& initializer) const{ 
    cout << "\n参照物A的实际位置和B的实际航迹:" << endl;
    GameRenderer::printGrid(initializer.getShipA().getTrajectory(), initializer.getShipB().getTrajectory(), initializer, false);
}

bool BaseGameMode2::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer){
    playerAnswer.clear(); // 清空之前的答案

    cout << "\n请输入B的相对航迹，共" << initializer.getRelativeB().size() << "步" << endl;
    
    // 逐步获取玩家输入
    for(size_t i = 0; i < initializer.getRelativeB().size(); i++){
        cout << "第" << (i+1) << "步 (格式: x,y): ";
        
        // 如果是闯关模式，显示剩余时间
        if(isStageMode){
            auto currentTime = chrono::steady_clock::now();
            auto elapsedTime = chrono::duration_cast<chrono::seconds>(
                currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
            int remainingTime = 150 - elapsedTime.count();
            if(remainingTime <= 0){
                cout << "\n时间已用尽！" << endl;
                return false;
            }
            cout << "[剩余时间: " << remainingTime << "秒] ";
        }
        
        string input;
        getline(cin, input);
        
        // 检查是否有暂停请求
        if(input == "p" || input == "P"){
            pauseGame();
            cout << "游戏已暂停，请输入 'p/P' 继续游戏" << endl;
            // 循环等待直到接收到继续游戏的命令
            while(true){
                getline(cin, input);
                if(input == "p" || input == "P"){
                    resumeGame();
                    cout << "游戏继续..." << endl;
                    break;
                }
                cout << "游戏仍处于暂停状态，请输入 'p/P' 继续游戏" << endl;
            } 
            i--;
            continue;
        }
        
        // 如果是闯关模式，检查是否超时
        if(isStageMode && isTimeExceeded()){
            cout << "\n时间已用尽！" << endl;
            return false;
        }

        // 检查作弊码
        if(input == "999"){
            playerAnswer = initializer.getRelativeB();
            break;
        }
        
        // 解析输入
        size_t comma = input.find(',');
        if(comma != string::npos){
            try{
                int x = stoi(input.substr(0, comma));
                int y = stoi(input.substr(comma+1));
                playerAnswer.emplace_back(x, y);
            } 
            catch(const exception& e){
                cout << "输入格式错误，请重新输入此步骤" << endl;
                i--;
                continue;
            }
        } 
        else{
            cout << "输入格式错误，请重新输入此步骤" << endl;
            i--; 
        }
    }
    
    // 验证答案
    for(size_t i = 0; i < initializer.getRelativeB().size(); i++){
        if(playerAnswer[i].x != initializer.getRelativeB()[i].x || 
           playerAnswer[i].y != initializer.getRelativeB()[i].y){
            return false;
        }
    }   
    return true;
}
void BaseGameMode2::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const{
    if(correct){
        cout << "正确！" << endl;
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

void BaseGameMode2::timeStart(){ timeEngine.start(); }
void BaseGameMode2::timeEnd(){ timeEngine.end(); }
chrono::seconds BaseGameMode2::getTimeCost(){ return timeEngine.getTimeCost(); }
void BaseGameMode2::displayTimeCost(){ timeEngine.displayTimeCost(); }

void BaseGameMode2::pauseGame(){ timeEngine.pause(); }
void BaseGameMode2::resumeGame(){ timeEngine.resume(); }

bool BaseGameMode2::isTimeExceeded(int timeLimit) const {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(
        currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
    return elapsedTime.count() > timeLimit;
}

BaseGameMode3::BaseGameMode3() : timeEngine(), isRelative(false){}

void BaseGameMode3::setIsRelative(){ isRelative = rand() % 2; }
bool BaseGameMode3::getIsRelative() const{ return isRelative; }

void BaseGameMode3::displayBaseModeInfo(GameInitializer& initializer) const{ cout << "游戏形式: 逐步推测B的航迹" << endl; }

void BaseGameMode3::displayGridInfo(GameInitializer& initializer) const{
    if(!getIsRelative()){ cout << "\n参照物A的实际位置和B的相对航迹:" << endl; } 
    else{ cout << "\n参照物A的实际位置和B的实际航迹:" << endl; }
}

bool BaseGameMode3::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer){
    cout << "\n请输入B的" << (getIsRelative() ? "相对" : "实际") << "航迹第" << (playerAnswer.size() + 1) << "步（格式：x,y）：";
    
    // 如果是闯关模式，显示剩余时间
    if(isStageMode){
        auto currentTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::seconds>(
            currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
        int remainingTime = 150 - elapsedTime.count();
        if(remainingTime <= 0){
            cout << "\n时间已用尽！" << endl;
            return false;
        }
        cout << "[剩余时间: " << remainingTime << "秒] ";
    }
    
    string input;
    getline(cin, input);

    // 检查是否有暂停请求
    if(input == "p" || input == "P"){
        pauseGame();
        cout << "游戏已暂停，请输入 'p/P' 继续游戏" << endl;        
        // 循环等待直到接收到继续游戏的命令
        while(true){
            getline(cin, input);
            if(input == "p" || input == "P"){
                resumeGame();
                cout << "游戏继续..." << endl;
                break;
            }
            cout << "游戏仍处于暂停状态，请输入 'p/P' 继续游戏" << endl;
        }      
        return processPlayerInput(playerAnswer, initializer);
    }
    
    // 如果是闯关模式，检查是否超时
    if(isStageMode && isTimeExceeded()){
        cout << "\n时间已用尽！" << endl;
        return false;
    }

    // 检查作弊码
    if(input == "999"){
        if(getIsRelative()){
            playerAnswer.push_back(initializer.getRelativeB()[playerAnswer.size()]);
        }
        else{
            playerAnswer.push_back(initializer.getShipB().getTrajectory()[playerAnswer.size()]);
        }
        return true;
    }
    
    // 解析输入
    size_t comma = input.find(',');
    if(comma != string::npos){
        try{
            int x = stoi(input.substr(0, comma));
            int y = stoi(input.substr(comma+1));
            playerAnswer.push_back(TrajectoryPoint(x, y));
        } 
        catch(const exception& e){
            cout << "输入格式错误，请使用正确的格式 (x,y)" << endl;
            return processPlayerInput(playerAnswer, initializer);
        }
    } 
    else{
        cout << "输入格式错误，请使用正确的格式 (x,y)" << endl;
        return processPlayerInput(playerAnswer, initializer);
    }
    
    // 验证答案
    if(getIsRelative()){
        return(playerAnswer.back().x == initializer.getRelativeB()[playerAnswer.size()-1].x && 
               playerAnswer.back().y == initializer.getRelativeB()[playerAnswer.size()-1].y);
    }
    else{
        return(playerAnswer.back().x == initializer.getShipB().getTrajectory()[playerAnswer.size()-1].x &&
               playerAnswer.back().y == initializer.getShipB().getTrajectory()[playerAnswer.size()-1].y);
    }
}
void BaseGameMode3::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, GameInitializer& initializer) const{
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
        cout << "挑战失败！" << endl;
    }
}
bool BaseGameMode3::playBaseGameMode(GameInitializer& initializer){
    // 开始计时
    timeStart();

    bool correct = true;
    vector<TrajectoryPoint> playerAnswer;
    cout << "\n游戏提示：输入 'p' 可随时暂停/继续游戏" << endl;
    
    for(size_t step = 1; step < initializer.getShipA().getTrajectory().size() && correct; ++step){
        // 随机决定这一步是猜实际航迹还是相对航迹
        setIsRelative();

        // 显示A的当前轨迹
        vector<TrajectoryPoint> currentA(initializer.getShipA().getTrajectory().begin(), initializer.getShipA().getTrajectory().begin() + step + 1);
        GameRenderer::printTrajectory(currentA, "A的实际");

        // 准备当前步骤的轨迹
        vector<TrajectoryPoint> currentB(initializer.getShipB().getTrajectory().begin(), initializer.getShipB().getTrajectory().begin() + step + 1);
        vector<TrajectoryPoint> currentRelativeB(initializer.getRelativeB().begin(), initializer.getRelativeB().begin() + step + 1);

        if(!getIsRelative()){
            // 如果要猜B的实际航迹，显示完整的相对航迹，但实际航迹少显示一步
            GameRenderer::printTrajectory(currentRelativeB, "B的相对");
            vector<TrajectoryPoint> prevB(initializer.getShipB().getTrajectory().begin(), initializer.getShipB().getTrajectory().begin() + step);
            GameRenderer::printTrajectory(prevB, "B的实际");
            displayGridInfo(initializer);
            GameRenderer::printGrid(currentA, currentRelativeB, initializer, true);
        } 
        else{
            // 如果要猜B的相对航迹，显示完整的实际航迹，但相对航迹少显示一步
            GameRenderer::printTrajectory(currentB, "B的实际");
            vector<TrajectoryPoint> prevRelativeB(initializer.getRelativeB().begin(), initializer.getRelativeB().begin() + step);
            GameRenderer::printTrajectory(prevRelativeB, "B的相对");
            displayGridInfo(initializer);
            GameRenderer::printGrid(currentA, currentB, initializer, false);
        }
        
        // 处理玩家输入
        correct = processPlayerInput(playerAnswer, initializer);

        // 显示当前步骤结果
        displayResult(correct, playerAnswer, initializer);
    }
    if(correct){
        cout << "挑战成功！" << endl;
    }
    //结束计时并显示用时
    timeEnd();
    displayTimeCost();
    return correct;
}

void BaseGameMode3::timeStart(){ timeEngine.start(); }
void BaseGameMode3::timeEnd(){ timeEngine.end(); }
chrono::seconds BaseGameMode3::getTimeCost(){ return timeEngine.getTimeCost(); }
void BaseGameMode3::displayTimeCost(){ timeEngine.displayTimeCost(); }

void BaseGameMode3::pauseGame(){ timeEngine.pause(); }
void BaseGameMode3::resumeGame(){ timeEngine.resume(); }

bool BaseGameMode3::isTimeExceeded(int timeLimit) const {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(
        currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
    return elapsedTime.count() > timeLimit;
}

BaseGameModeEngine::BaseGameModeEngine(){
    randomSetBaseGameMode();
    setCurrentBaseGameMode();
}
BaseGameModeEngine::BaseGameModeEngine(int stageChoice){
    setBaseGameMode(stageChoice);
    setCurrentBaseGameMode();
}
BaseGameModeEngine::~BaseGameModeEngine(){
    delete currentBaseGameMode;
}

void BaseGameModeEngine::setBaseGameMode(int stageChoice){ 
    if(stageChoice <= 2){ baseGameMode = 1; }
    else if(stageChoice <= 4){ baseGameMode = 0; }
    else if(stageChoice <= 6){ baseGameMode = 2; }
    else{ baseGameMode = rand() % 3; }
}
void BaseGameModeEngine::randomSetBaseGameMode(){ baseGameMode = rand() % 3; }
void BaseGameModeEngine::setCurrentBaseGameMode(){
    switch(getBaseGameMode()){
        case 0:
            currentBaseGameMode = new BaseGameMode1();
            break;
        case 1:
            currentBaseGameMode = new BaseGameMode2();
            break;
        case 2:
            currentBaseGameMode = new BaseGameMode3();
            break;
    }
}
AbstractBaseGameMode* BaseGameModeEngine::getCurrentBaseMode(){ return currentBaseGameMode; }

int BaseGameModeEngine::getBaseGameMode(){ return baseGameMode; }
chrono::seconds BaseGameModeEngine::getTimeCost(){ return currentBaseGameMode -> getTimeCost(); }
bool BaseGameModeEngine::startBaseGameMode(GameInitializer& initializer){
    // 显示游戏信息
    cout << "网格大小: " << initializer.getGridSize() << "x" << initializer.getGridSize() << endl;
    cout << "航迹步数: " << initializer.getSteps() << endl;

    // 执行游戏
    return getCurrentBaseMode()->playBaseGameMode(initializer);
}

GameInitializer::GameInitializer(Player& player)
     : gridSize(0), steps(0), difficulty(0), shipA(0, 0), shipB(0, 0), baseGameModeEngine(){
        setChallengeMaxDifficulty(player);
        generateChallengeSteps();
        calculateChallengeDifficulty();
        generateGridSize();
        initializeAllTrajectory();
     }
GameInitializer::GameInitializer(int stageChoice, Player& player)
     : gridSize(0), steps(0), difficulty(0), shipA(0, 0), shipB(0, 0), baseGameModeEngine(stageChoice){
        generateStageSteps(stageChoice);
        generateGridSize();
        initializeAllTrajectory();
     }

void GameInitializer::setChallengeMaxDifficulty(Player& player){
    if(player.getChallengeRank() == PlayerRank::BRONZE){ maxDifficulty = 1; }
    else if(player.getChallengeRank() == PlayerRank::SILVER){ maxDifficulty = 2; }
    else if(player.getChallengeRank() == PlayerRank::GOLD){ maxDifficulty = 3; }
    else if(player.getChallengeRank() == PlayerRank::PLATINUM){ maxDifficulty = 4; }
    else if(player.getChallengeRank() == PlayerRank::DIAMOND){ maxDifficulty = 5; }
    else if(player.getChallengeRank() == PlayerRank::MASTER){ maxDifficulty = 6; }
}
void GameInitializer::generateChallengeSteps(){
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
void GameInitializer::calculateChallengeDifficulty(){
    if(baseGameModeEngine.getBaseGameMode() == 0 || baseGameModeEngine.getBaseGameMode() == 1){
        difficulty = ceil((double)(steps - 4) / 2);
    }
    else if(baseGameModeEngine.getBaseGameMode() == 2){
        if(steps <= 9) { difficulty = ceil((double)(steps - 3) / 2); }
        else if(steps <= 12) { difficulty = ceil((double)(steps - 2) / 2); }
        else { difficulty = 6; }
    }
}

void GameInitializer::generateStageSteps(int stageChoice){
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

void GameInitializer::generateGridSize(){ 
    gridSize = 4 + 4 * steps / 7 + rand() % 2;  
}
pair<int, int> GameInitializer::getRandomStart(){
    return { rand() % getGridSize(), rand() % getGridSize() };
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
void GameInitializer::initializeAllTrajectory(){
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

const int& GameInitializer::getGridSize() const { return gridSize; }
const int& GameInitializer::getSteps() const { return steps; }
const int& GameInitializer::getDifficulty() const { return difficulty; }
const int& GameInitializer::getPoints() const { return difficulty; }
const Ship& GameInitializer::getShipA() const { return shipA; }
const Ship& GameInitializer::getShipB() const { return shipB; }
const vector<TrajectoryPoint>& GameInitializer::getRelativeB() const { return relativeB; }
int GameInitializer::getChallengeMaxDifficulty() const{ return maxDifficulty; }
BaseGameModeEngine& GameInitializer::getBaseGameModeEngine() { return baseGameModeEngine; }

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

bool PlayerManager::registerPlayer(const string& username, const string& password){
    // 检查用户名是否已存在
    ifstream inFile(PLAYER_DATA_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int score;
            ss >> name >> pwd >> score;
            
            if(name == username){
                cout << "用户名已存在，请选择其他用户名。" << endl;
                inFile.close();
                return false;
            }
        }
        inFile.close();
    }
    
    // 添加新玩家到文件
    ofstream outFile(PLAYER_DATA_FILE, ios::app);
    if(outFile.is_open()){
        outFile << username << " " << password << " 0 0 0 0" << endl;
        outFile.close();
        cout << "注册成功！" << endl;
        return true;
    } 
    else{
        cout << "无法打开玩家数据文件，注册失败。" << endl;
        return false;
    }
}
bool PlayerManager::loginPlayer(const string& username, const string& password, Player& player){
    ifstream inFile(PLAYER_DATA_FILE);
    bool userFound = false;
    
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            int challengeScore, challengeTotal, challengeSuccess, currentStage;
            ss >> name >> pwd >> challengeScore >> challengeTotal >> challengeSuccess >> currentStage;
            
            if(name == username){
                userFound = true;
                if(pwd == password){
                    // 初始化挑战模式数据
                    player = Player(username, password); 
                    player.setChallengeScore(challengeScore);
                    player.calculateRank();
                    player.setTotalChallengeGames(challengeTotal);
                    player.setSuccessChallengeGames(challengeSuccess);
                    player.setCurrentStage(currentStage);
                    
                    // 读取关卡用时记录
                    string timeRecord;
                    while(ss >> timeRecord){
                        size_t colonPos = timeRecord.find(':');
                        if(colonPos != string::npos){
                            int stage = stoi(timeRecord.substr(0, colonPos));
                            int time = stoi(timeRecord.substr(colonPos + 1));
                            player.setStageTime(stage, time);
                        }
                    }

                    cout << "登录成功！" << endl;
                    cout << "--------------------" << endl;
                    cout << "挑战模式积分: " << challengeScore << endl;
                    cout << "挑战模式等级: " << player.getChallengeRankName() << endl;
                    cout << "挑战模式成功率: " << player.getChallengeSuccessRate() * 100 << "%" << endl;
                    cout << "闯关模式已闯关卡: " << player.getCurrentStage() << endl;
                    cout << "--------------------" << endl;
                    return true;
                } 
                else{ break; } // 用户名找到但密码错误，停止搜索
            }
        }
        inFile.close();
        
        if(userFound){ cout << "密码错误。" << endl; } 
        else { cout << "用户名不存在。" << endl; }
    } 
    else { cout << "无法打开玩家数据文件，登录失败。" << endl; }
    return false;
}
void PlayerManager::updatePlayerData(const Player& player){
    string line;
    vector<string> lines;
    ifstream inFile(PLAYER_DATA_FILE);
    
    if(inFile.is_open()){
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, pwd;
            ss >> name >> pwd;

            if(name == player.getUsername()){
                // 更新该玩家的数据行
                line = name + " " + pwd + 
                       " " + to_string(player.getChallengeScore()) +
                       " " + to_string(player.getTotalChallengeGames()) +
                       " " + to_string(player.getSuccessChallengeGames()) +
                       " " + to_string(player.getCurrentStage());
                
                for(int stage = 1; stage <= 8; stage++){
                    if(player.hasStageTimeRecord(stage)){
                        line += " " + to_string(stage) + ":" + to_string(player.getStageTime(stage));
                    }
                }
            }
            lines.push_back(line);
        }
        inFile.close();
        
        // 重新写入文件
        ofstream outFile(PLAYER_DATA_FILE);
        for(const auto& l : lines){ outFile << l << endl; }
        outFile.close();
    }
    else{ cout << "无法打开玩家数据文件，数据保存失败" << endl; }
}

void LeaderboardManager::updateChallengeLeaderboard(const Player& player){
    vector<tuple<string, int, string, double>> players;
    bool playerFound = false;
    
    // 读取现有排行榜
    ifstream inFile(CHALLENGE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, rank;
            int score;
            double rate;
            ss >> name >> score >> rank >> rate;
            
            if(name == player.getUsername()){ continue; }
            players.push_back({name, score, rank, rate});
        }
        inFile.close();
    }
    
    // 添加当前玩家数据
    players.push_back({player.getUsername(), 
                      player.getChallengeScore(), 
                      player.getChallengeRankName(), 
                      player.getChallengeSuccessRate()});
    
    // 按分数降序排序，同分按成功率降序
    sort(players.begin(), players.end(), 
         [](const auto& a, const auto& b){
             if(get<1>(a) != get<1>(b)){
                 return get<1>(a) > get<1>(b);
             }
             return get<3>(a) > get<3>(b);  // 比较成功率
         });
    
    // 写回文件
    ofstream outFile(CHALLENGE_LEADERBOARD_FILE);
    if(outFile.is_open()){
        for(const auto& p : players){
            outFile << get<0>(p) << " " << get<1>(p) << " " 
                   << get<2>(p) << " " << get<3>(p) << endl;
        }
        outFile.close();
    } 
    else{
        cout << "无法打开排行榜文件，保存失败" << endl;
    }
}
void LeaderboardManager::displayChallengeLeaderboard(){
    vector<tuple<string, int, string, double>> players; 
    
    ifstream inFile(CHALLENGE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, rank;
            int score;
            double rate;
            ss >> name >> score >> rank >> rate;
            
            players.push_back({name, score, rank, rate});
        }
        inFile.close();
        
        cout << "\n===== 挑战积分排行榜 =====" << endl;
        cout << "排名\t用户名\t积分\t等级\t成功率" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << get<0>(players[i]) << "\t" 
                 << get<1>(players[i]) << "\t"
                 << get<2>(players[i]) << "\t"
                 << fixed << setprecision(1) << get<3>(players[i]) << "%" << endl;
        }
        
        if(players.empty()){
            cout << "暂无玩家数据" << endl;
        }
    } 
    else{
        cout << "无法打开排行榜文件。" << endl;
    }
}

void LeaderboardManager::updateStageLeaderboard(const Player& player){
    vector<tuple<string, int>> players;
    bool playerFound = false;
    
    // 读取现有排行榜
    ifstream inFile(STAGE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name, rank;
            int challengeScore, challengeTotal, challengeSuccess, currentStage;
            ss >> name >> challengeScore >> challengeTotal >> challengeSuccess >> currentStage;
            
            if(name == player.getUsername()){ continue; }
            players.push_back({name, currentStage});
        }
        inFile.close();
    }
    
    // 添加当前玩家数据
    players.push_back({player.getUsername(), player.getCurrentStage()});
    
    // 按分数降序排序，同分按成功率降序
    sort(players.begin(), players.end(), 
         [](const auto& a, const auto& b){
             return get<1>(a) > get<1>(b);
         });
    
    // 写回文件
    ofstream outFile(STAGE_LEADERBOARD_FILE);
    if(outFile.is_open()){
        for(const auto& p : players){
            outFile << get<0>(p) << " " << get<1>(p) << endl;
        }
        outFile.close();
    } 
    else{
        cout << "无法打开排行榜文件，保存失败" << endl;
    }
}
void LeaderboardManager::displayStageLeaderboard(){
    vector<tuple<string, int>> players;
    
    ifstream inFile(STAGE_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int currentStage;
            ss >> name >> currentStage;
            
            players.push_back({name, currentStage});
        }
        inFile.close();
        
        cout << "\n===== 闯关排行榜 =====" << endl;
        cout << "排名\t用户名\t已闯过关卡" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << get<0>(players[i]) << "\t" 
                 << get<1>(players[i]) << endl;
        }
        
        if(players.empty()){
            cout << "暂无玩家数据" << endl;
        }
    } 
    else{
        cout << "无法打开排行榜文件。" << endl;
    }
}

void LeaderboardManager::updateStageTimeLeaderboard(const Player& player, int stage, int timeSeconds){
    vector<tuple<string, int>> players;
    bool playerFound = false;
    
    string filename = LEVEL_TIME_FILE + to_string(stage) + ".txt";
    
    ifstream inFile(filename);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int time;
            ss >> name >> time;
            
            if(name == player.getUsername()){ 
                playerFound = true;
                // 如果已有记录且新时间更短，则更新
                if(timeSeconds < time){ players.push_back({name, timeSeconds}); } 
                else{ players.push_back({name, time}); }
            } 
            else { players.push_back({name, time}); }
        }
        inFile.close();
    }
    
    // 如果玩家不在排行榜中，添加新记录
    if(!playerFound){ players.push_back({player.getUsername(), timeSeconds}); }
    
    // 按用时升序排序（用时越短越好）
    sort(players.begin(), players.end(), 
         [](const auto& a, const auto& b){
             return get<1>(a) < get<1>(b);
         });
    
    // 写回文件
    ofstream outFile(filename);
    if(outFile.is_open()){
        for(const auto& p : players) {
            outFile << get<0>(p) << " " << get<1>(p) << endl;
        }
        outFile.close();
    } 
    else{ cout << "无法打开排行榜文件，保存失败" << endl; }
}
void LeaderboardManager::displayStageTimeLeaderboard(int stage){
    vector<tuple<string, int>> players;
    
    string filename = LEVEL_TIME_FILE + to_string(stage) + ".txt";
    
    ifstream inFile(filename);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int time;
            ss >> name >> time;
            
            players.push_back({name, time});
        }
        inFile.close();
        
        cout << "\n===== 第" << stage << "关用时排行榜 =====" << endl;
        cout << "排名\t用户名\t用时(秒)" << endl;
        
        for(size_t i = 0; i < players.size(); ++i){
            cout << i + 1 << "\t" 
                 << get<0>(players[i]) << "\t" 
                 << get<1>(players[i]) << endl;
        }
        
        if(players.empty()){ cout << "暂无玩家数据" << endl; }
    } 
    else{ cout << "暂无该关卡的排行榜数据。" << endl; }
}
void LeaderboardManager::displayStageTimeLeaderboardMenu(){
    int stageChoice;
    cout << "\n请选择要查看的关卡用时排行榜（1-8）：";
    cin >> stageChoice;
    cin.ignore();
    
    if(stageChoice >= 1 && stageChoice <= 8){
        displayStageTimeLeaderboard(stageChoice);
    } 
    else{ cout << "无效的关卡选择。" << endl; }
}

void ChallengeMode::playChallengeMode(Player& player){
    // 初始化游戏
    GameInitializer initializer(player);
    // 显示游戏信息
    cout << "游戏难度: " << initializer.getDifficulty() << endl;
    // 执行游戏
    bool result = initializer.getBaseGameModeEngine().startBaseGameMode(initializer);
    // 更新玩家游戏记录
    player.addChallengeGame(result);

    // 显示玩家游戏结果
    if(result){
        cout << "你获得了 " << 2 * initializer.getPoints() << " 分！" << endl;
        player.addChallengeScore(2 * initializer.getPoints());
    }
    else{
        if(player.getChallengeScore() > initializer.getPoints()){
            cout << "你失去了 " << initializer.getPoints() << " 分。" << endl;
            player.reduceChallengeScore(initializer.getPoints());
        } 
        else if(player.getChallengeScore() > 0){
            cout << "你失去了 " << player.getChallengeScore() << " 分。" << endl;
            player.setChallengeScore(0);
        } 
        else{
            cout << "你的分数已经为0，不再扣分。" << endl;
        }
    }    

    // 更新玩家数据
    PlayerManager::updatePlayerData(player);
    LeaderboardManager::updateChallengeLeaderboard(player);
}

void StageMode::displayAvailableStage(Player& player){
    cout << "当前可闯关卡: " << endl;
    for(int i = 1; i <= player.getCurrentStage() + 1; i++){
        cout << i << " " << endl;
    }
}
void StageMode::playStageMode(Player& player){
    int stageChoice;
    bool validChoice = true;
    while(validChoice){
        displayAvailableStage(player);
        cout << "请选择闯关模式: ";
        cin >> stageChoice;
        cin.ignore();
    
        if(stageChoice > player.getCurrentStage() + 1){
            cout << "当前关卡尚未解锁，请先闯过前面的关卡。" << endl;
        }
        else{ validChoice = false; }
    }
    // 初始化游戏
    GameInitializer initializer(stageChoice, player);
    
    // 设置为闯关模式（启用时间限制）
    if(auto mode1 = dynamic_cast<AbstractBaseGameMode*>(initializer.getBaseGameModeEngine().getCurrentBaseMode())){
        mode1->setIsStageMode(true);
    } 
    else if(auto mode2 = dynamic_cast<AbstractBaseGameMode*>(initializer.getBaseGameModeEngine().getCurrentBaseMode())){
        mode2->setIsStageMode(true);
    } 
    else if(auto mode3 = dynamic_cast<AbstractBaseGameMode*>(initializer.getBaseGameModeEngine().getCurrentBaseMode())){
        mode3->setIsStageMode(true);
    }
    
    cout << "【闯关模式】每个关卡限时150秒，超时将视为挑战失败！" << endl;
    
    // 执行游戏
    bool result = initializer.getBaseGameModeEngine().startBaseGameMode(initializer);
    // 获取闯关用时
    int timeCost = initializer.getBaseGameModeEngine().getTimeCost().count();

    // 显示玩家游戏结果
    if(result){ 
        if(timeCost <= 150){
            cout << "闯关成功！" << endl;
            cout << "本次用时: " << timeCost << " 秒" << endl;
            
            // 如果通过新关卡，更新当前关卡进度
            if(stageChoice > player.getCurrentStage()){
                player.setCurrentStage(stageChoice);
            }
            
            // 更新用时记录
            player.setStageTime(stageChoice, timeCost);
            
            // 更新排行榜
            LeaderboardManager::updateStageTimeLeaderboard(player, stageChoice, timeCost);
        } 
        else{
            cout << "时间超过150秒限制，闯关失败！" << endl;
        }
    }
    else{ 
        cout << "闯关失败!" << endl; 
        if(timeCost > 150){
            cout << "超过时间限制(150秒)，请尝试更快完成。" << endl;
        }
    }

    // 更新玩家数据
    PlayerManager::updatePlayerData(player);
    LeaderboardManager::updateStageLeaderboard(player);
}

void GameRunner::runGame(){
    // 用当前时间作为随机数种子
    srand(static_cast<unsigned int>(time(0)));

    // 玩家登录/注册
    Player currentPlayer;
    bool continueGame = true;
    
    while(continueGame){
        bool loggedIn = false;
        bool playAgain = true;
        while(!loggedIn){
            cout << "===== 欢迎来到脑力航迹游戏 =====" << endl;
            cout << "1. 登录" << endl;
            cout << "2. 注册" << endl;
            cout << "3. 退出游戏" << endl;
            cout << "请选择操作: ";

            int choice;
            cin >> choice;
            cin.ignore(); // 清除输入缓冲区中的换行符

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
                        
                        if(loginChoice == 'y' || loginChoice == 'Y'){
                            loggedIn = PlayerManager::loginPlayer(username, password, currentPlayer);
                        }
                    }
                    break;
                case 3: // 退出
                    cout << "感谢游玩，再见！" << endl;
                    playAgain = false;
                    continueGame = false;
                    break;
                default:
                    cout << "无效选择，请重试。" << endl;
            }
        }

        // 游戏主循环
        while(playAgain){
            // 主菜单
            cout << "\n===== 主菜单 =====" << endl;
            cout << "1. 开始游戏" << endl;
            cout << "2. 查看排行榜" << endl;
            cout << "3. 返回登录注册页面" << endl;
            cout << "4. 退出游戏" << endl;
            cout << "请选择: ";
            
            int choice;
            cin >> choice;
            cin.ignore(); 

            switch(choice){
                case 1:{
                    cout << "\n===== 游戏操作说明 =====" << endl;
                    cout << "- 在游戏过程中，您可以按 'P' 键暂停游戏" << endl;
                    cout << "- 暂停后，按 'R' 键继续游戏" << endl;
                    cout << "- 暂停期间计时会停止，不影响您的游戏成绩" << endl;
                    bool invalidChoice = true;
                    while(invalidChoice){ 
                        // 获取玩家选择的游戏模式
                        cout << "\n===== 游戏模式选择 =====" << endl;
                        cout << "1. 挑战模式 (积分: " << currentPlayer.getChallengeScore() 
                            << ", 等级: " << currentPlayer.getChallengeRankName() << ")" << endl;
                        cout << "2. 闯关模式" << endl;
                        cout << "3. 娱乐模式 (积分: " << ")" << endl;
                        cout << "4. 返回主菜单" << endl;
                        cout << "请选择: ";

                        int modeChoice;
                        cin >> modeChoice;
                        cin.ignore(); 
                        
                        switch(modeChoice){
                            case 1: // 挑战模式
                                invalidChoice = false;
                                ChallengeMode::playChallengeMode(currentPlayer);
                                break;
                            case 2: // 闯关模式
                                invalidChoice = false;
                                StageMode::playStageMode(currentPlayer);
                                break;
                            case 3: // 娱乐模式
                                invalidChoice = false;
                                break;
                            case 4: // 返回主菜单
                                invalidChoice = false;
                                break;
                            default:
                                cout << "无效选择，请重新选择。" << endl;
                                break;
                        }
                    }
                    break;
                }
                case 2:{
                    bool invalidChoice = true;
                    while(invalidChoice){
                        cout << "选择要查看的排行榜：" << endl;
                        cout << "1. 挑战模式排行榜" << endl;
                        cout << "2. 闯关模式排行榜" << endl;
                        cout << "3. 娱乐模式排行榜" << endl;
                        cout << "请选择: ";
                        
                        int leaderboardChoice;
                        cin >> leaderboardChoice;
                        cin.ignore(); 
                        
                        switch(leaderboardChoice){
                            case 1:
                                invalidChoice = false;
                                LeaderboardManager::displayChallengeLeaderboard();
                                break;
                            case 2:
                                invalidChoice = false;
                                // 闯关模式排行榜子菜单
                                int stageLeaderboardChoice;
                                cout << "\n选择要查看的闯关排行榜类型：" << endl;
                                cout << "1. 闯关数排行榜" << endl;
                                cout << "2. 闯关用时排行榜" << endl;
                                cout << "请选择: ";
                                cin >> stageLeaderboardChoice;
                                cin.ignore();
                                
                                if(stageLeaderboardChoice == 1){
                                    LeaderboardManager::displayStageLeaderboard();
                                } 
                                else if(stageLeaderboardChoice == 2){
                                    LeaderboardManager::displayStageTimeLeaderboardMenu();
                                } 
                                else { cout << "无效选择" << endl; }
                                break;
                            default:
                                cout << "无效选择，请重新选择。" << endl;
                                break;
                        }
                    }
                    break;
                }
                case 3: // 返回游戏登录注册页面
                    loggedIn = false;
                    playAgain = false;
                    break;
                case 4: // 退出游戏
                    cout << "感谢使用，再见！" << endl;
                    playAgain = false;
                    continueGame = false;
                    break;
                default:
                    cout << "无效选择，请重新选择。" << endl;
            }
        }
    }
}
