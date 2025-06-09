#include "Mental_Trajectory.h"

bool AbstractBaseGameMode::playBaseGameMode(AbstractInitializer* initializer){
    vector<TrajectoryPoint> playerAnswer;
    timeStart();
    displayBaseModeInfo(initializer);
    displayGridInfo(initializer);
    
    // 处理玩家输入
    bool correct = processPlayerInput(playerAnswer, initializer);
    
    timeEnd();
    displayTimeCost();
    displayResult(correct, playerAnswer, initializer);
    return correct;
}
bool AbstractBaseGameMode::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer, const function<const vector<TrajectoryPoint>&()>& getTrajectory){
    playerAnswer.clear();
    const auto& trajectory = getTrajectory(); // 通过传入的函数获取数据

    cout << "\n===== 请逐步输入B的航迹,共" << trajectory.size() << "步=====" << endl;

    for(size_t i = 0; i < trajectory.size(); i++){
        cout << "第" << (i + 1) << "步 (格式: x,y): ";

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

        // 作弊接口
        if(input == "999"){
            playerAnswer = trajectory;
            return true;
        }

        if(input == "p" || input == "P"){
            pauseGame();
            cout << "游戏已暂停，请输入 'p/P' 继续游戏" << endl;
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
        if(input == "h" || input == "H"){
            if(isStageMode){
                useHint(initializer, playerAnswer);
            }
            else{
                cout << "提示功能仅在闯关模式下可用！" << endl;
            }
            i--;
            continue;
        }
        
        if(isStageMode && isTimeExceeded()){
            cout << "\n时间已用尽！" << endl;
            return false;
        }

        // 解析输入并添加到 playerAnswer
        size_t comma = input.find(',');
        if (comma != string::npos) {
            try {
                int x = stoi(input.substr(0, comma));
                int y = stoi(input.substr(comma + 1));
                playerAnswer.emplace_back(x, y);
            } catch (const exception& e) {
                cout << "输入格式错误，请重新输入此步骤" << endl;
                i--;
                continue;
            }
        } else {
            cout << "输入格式错误，请重新输入此步骤" << endl;
            i--;
        }
    }

    // 验证答案
    for (size_t i = 0; i < trajectory.size(); i++) {
        if (playerAnswer[i].x != trajectory[i].x ||
            playerAnswer[i].y != trajectory[i].y) {
            return false;
        }
    }
    return true;
} 
void AbstractBaseGameMode::useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer, const function<const TrajectoryPoint&(size_t)> getSteps){
    if(hintUsed){
        cout << "你已经使用过提示了，每关只能使用一次提示！" << endl;
        return;
    }

    size_t currentStep = playerAnswer.size();
    
    // 通过传入的函数获取正确步骤
    const TrajectoryPoint& correctStep = getSteps(currentStep);

    cout << "\n==== 提示信息 ====" << endl;
    cout << "航迹第" << (currentStep + 1) << "步为: ("
         << correctStep.x << ","
         << correctStep.y << ")" << endl;

    applyHintTimePenalty();
}
void AbstractBaseGameMode::setIsStageMode(bool isStageMode){
    this->isStageMode = isStageMode;
}
bool AbstractBaseGameMode::isTimeExceeded(int timeLimit) const {
    auto currentTime = chrono::steady_clock::now();
    auto elapsedTime = chrono::duration_cast<chrono::seconds>(
        currentTime - timeEngine.getStartTime() - timeEngine.getPausedDuration());
    return elapsedTime.count() > timeLimit;
}

BaseGameMode1::BaseGameMode1(){}

void BaseGameMode1::displayBaseModeInfo(AbstractInitializer* initializer) const{ 
    cout << "\n===== 推测B的实际航迹 =====" << endl; 
    GameRenderer::printTrajectory(initializer->getShipA().getTrajectory(), "A的实际");
    GameRenderer::printTrajectory(initializer->getRelativeB(), "B的相对");
}
void BaseGameMode1::displayGridInfo(AbstractInitializer* initializer) const{ 
    cout << "\n参照物A的实际位置和B的相对航迹:" << endl; 
    GameRenderer::printGrid(initializer->getShipA().getTrajectory(), initializer->getRelativeB(), initializer, true);
}

bool BaseGameMode1::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer){
    // 提供一个 lambda 函数，它捕获 initializer 并返回 B 的实际航迹
    return AbstractBaseGameMode::processPlayerInput(playerAnswer, initializer, 
        [initializer]() -> const vector<TrajectoryPoint>& { 
            return initializer->getShipB().getTrajectory(); 
        }
    );
}
void BaseGameMode1::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const{
    GameRenderer::printTrajectory(playerAnswer, "\n[您的答案]B的实际");
    GameRenderer::printTrajectory(initializer->getShipB().getTrajectory(), "[正确答案]B的实际");
    cout << "\n正确的最终位置图示:" << endl;
    GameRenderer::printGrid(initializer->getShipA().getTrajectory(), initializer->getShipB().getTrajectory(), initializer, false);
    cout << "\n==== 游戏结果 ====" << endl;
    if(correct){ cout << "恭喜! 挑战成功！" << endl; } 
    else{ cout << "很遗憾，挑战失败！" << endl; }
}

void BaseGameMode1::applyHintTimePenalty(){
    if(isStageMode && !hintUsed){
        // 增加虚拟的暂停时间，相当于减少15秒的可用时间
        auto penaltyTime = chrono::seconds(15);
        timeEngine.reducePausedDuration(penaltyTime); // 减少暂停时间，相当于减少已用时间
        hintUsed = true;
        cout << "已使用提示，剩余时间减少15秒！" << endl;
    }
}

void BaseGameMode1::useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer){
    // 提供一个 lambda 函数，用于获取 B 实际航迹的特定步骤
    AbstractBaseGameMode::useHint(initializer, playerAnswer,
        [initializer](size_t step) -> const TrajectoryPoint& {
            return initializer->getShipB().getTrajectory()[step];
        }
    );
}

BaseGameMode2::BaseGameMode2(){}

void BaseGameMode2::displayBaseModeInfo(AbstractInitializer* initializer) const{ 
    cout << "\n===== 推测B的相对航迹 =====" << endl;
    GameRenderer::printTrajectory(initializer->getShipA().getTrajectory(), "A的实际");
    GameRenderer::printTrajectory(initializer->getShipB().getTrajectory(), "B的实际"); 
}
void BaseGameMode2::displayGridInfo(AbstractInitializer* initializer) const{ 
    cout << "\n参照物A的实际位置和B的实际航迹:" << endl;
    GameRenderer::printGrid(initializer->getShipA().getTrajectory(), initializer->getShipB().getTrajectory(), initializer, false);
}

bool BaseGameMode2::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer){
    return AbstractBaseGameMode::processPlayerInput(playerAnswer, initializer,
        [initializer]() -> const vector<TrajectoryPoint>& {
            return initializer->getRelativeB();
        }
    );
}

void BaseGameMode2::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const{
    GameRenderer::printTrajectory(playerAnswer, "[您的答案]B的相对");
    GameRenderer::printTrajectory(initializer->getRelativeB(), "[正确答案]B的相对");
    cout << "\n正确的最终位置图示:" << endl;
    // 显示A的实际轨迹和B的相对轨迹
    GameRenderer::printGrid(initializer->getShipA().getTrajectory(), initializer->getRelativeB(), initializer, true);
    cout << "\n==== 游戏结果 ====" << endl;
    if(correct){
        cout << "恭喜! 挑战成功！" << endl;
    } 
    else{
        cout << "很遗憾，挑战失败！" << endl;
    }
}

void BaseGameMode2::applyHintTimePenalty(){
    if(isStageMode && !hintUsed){
        // 增加虚拟的暂停时间，相当于减少15秒的可用时间
        auto penaltyTime = chrono::seconds(15);
        timeEngine.reducePausedDuration(penaltyTime); // 减少暂停时间，相当于减少已用时间
        hintUsed = true;
        cout << "已使用提示，剩余时间减少15秒！" << endl;
    }
}

void BaseGameMode2::useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer){
    // 提供一个 lambda 函数，用于获取 B 相对航迹的特定步骤
    AbstractBaseGameMode::useHint(initializer, playerAnswer,
        [initializer](size_t step) -> const TrajectoryPoint& {
            return initializer->getRelativeB()[step];
        }
    );
}

BaseGameMode3::BaseGameMode3() : isRelative(false){}

void BaseGameMode3::setIsRelative(){ isRelative = rand() % 2; }
bool BaseGameMode3::getIsRelative() const{ return isRelative; }

void BaseGameMode3::displayBaseModeInfo(AbstractInitializer* initializer) const{ cout << "\n===== 逐步推测B的航迹 =====" << endl; }

void BaseGameMode3::displayGridInfo(AbstractInitializer* initializer) const{
    if(!getIsRelative()){ cout << "\n参照物A的实际位置和B的相对航迹:" << endl; } 
    else{ cout << "\n参照物A的实际位置和B的实际航迹:" << endl; }
}

bool BaseGameMode3::processPlayerInput(vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer){
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
    
    // 检查是否请求提示
    if(input == "h" || input == "H"){
        useHint(initializer, playerAnswer);
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
            playerAnswer.push_back(initializer->getRelativeB()[playerAnswer.size()]);
        }
        else{
            playerAnswer.push_back(initializer->getShipB().getTrajectory()[playerAnswer.size()]);
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
        return(playerAnswer.back().x == initializer->getRelativeB()[playerAnswer.size()-1].x && 
               playerAnswer.back().y == initializer->getRelativeB()[playerAnswer.size()-1].y);
    }
    else{
        return(playerAnswer.back().x == initializer->getShipB().getTrajectory()[playerAnswer.size()-1].x &&
               playerAnswer.back().y == initializer->getShipB().getTrajectory()[playerAnswer.size()-1].y);
    }
}
void BaseGameMode3::displayResult(bool correct, const vector<TrajectoryPoint>& playerAnswer, AbstractInitializer* initializer) const{
    if(correct){
        cout << "\n——————[正确]继续下一步——————" << endl;
    } 
    else{
        cout << "\n[错误]正确的" << (getIsRelative() ? "相对" : "实际") << "位置应该是：";
        if(getIsRelative()){
            cout << initializer->getRelativeB()[playerAnswer.size()-1].x << "," << initializer->getRelativeB()[playerAnswer.size()-1].y << endl;
        } 
        else{
            cout << initializer->getShipB().getTrajectory()[playerAnswer.size()-1].x << "," << initializer->getShipB().getTrajectory()[playerAnswer.size()-1].y << endl;
        }
    }
}
bool BaseGameMode3::playBaseGameMode(AbstractInitializer* initializer){
    // 开始计时
    timeStart();

    bool correct = true;
    vector<TrajectoryPoint> playerAnswer;
    
    for(size_t step = 0; step < initializer->getShipA().getTrajectory().size() && correct; ++step){
        // 随机决定这一步是猜实际航迹还是相对航迹
        setIsRelative();

        // 显示A的当前轨迹
        vector<TrajectoryPoint> currentA(initializer->getShipA().getTrajectory().begin(), initializer->getShipA().getTrajectory().begin() + step + 1);
        GameRenderer::printTrajectory(currentA, "\nA的实际");

        // 准备当前步骤的轨迹
        vector<TrajectoryPoint> currentB(initializer->getShipB().getTrajectory().begin(), initializer->getShipB().getTrajectory().begin() + step + 1);
        vector<TrajectoryPoint> currentRelativeB(initializer->getRelativeB().begin(), initializer->getRelativeB().begin() + step + 1);

        if(!getIsRelative()){
            // 如果要猜B的实际航迹，显示完整的相对航迹，但实际航迹少显示一步
            GameRenderer::printTrajectory(currentRelativeB, "B的相对");
            vector<TrajectoryPoint> prevB(initializer->getShipB().getTrajectory().begin(), initializer->getShipB().getTrajectory().begin() + step);
            GameRenderer::printTrajectory(prevB, "B的实际");
            displayGridInfo(initializer);
            GameRenderer::printGrid(currentA, currentRelativeB, initializer, true);
        } 
        else{
            // 如果要猜B的相对航迹，显示完整的实际航迹，但相对航迹少显示一步
            GameRenderer::printTrajectory(currentB, "B的实际");
            vector<TrajectoryPoint> prevRelativeB(initializer->getRelativeB().begin(), initializer->getRelativeB().begin() + step);
            GameRenderer::printTrajectory(prevRelativeB, "B的相对");
            displayGridInfo(initializer);
            GameRenderer::printGrid(currentA, currentB, initializer, false);
        }
        
        // 处理玩家输入
        correct = processPlayerInput(playerAnswer, initializer);

        // 显示当前步骤结果
        displayResult(correct, playerAnswer, initializer);
    }
    //结束计时并显示用时
    timeEnd();
    displayTimeCost();
    cout << "\n==== 游戏结果 ====" << endl;
    if(correct){ cout << "恭喜! 挑战成功！" << endl; } 
    else{ cout << "很遗憾，挑战失败！" << endl; }
    
    return correct;
}

void BaseGameMode3::applyHintTimePenalty(){
    if(isStageMode && !hintUsed){
        // 增加虚拟的暂停时间，相当于减少15秒的可用时间
        auto penaltyTime = chrono::seconds(15);
        timeEngine.reducePausedDuration(penaltyTime); // 减少暂停时间，相当于减少已用时间
        hintUsed = true;
        cout << "已使用提示，剩余时间减少15秒！" << endl;
    }
}

void BaseGameMode3::useHint(AbstractInitializer* initializer, vector<TrajectoryPoint>& playerAnswer){
    if(hintUsed){
        cout << "你已经使用过提示了，每关只能使用一次提示！" << endl;
        return;
    }
    
    // 获取当前输入步骤
    size_t currentStep = playerAnswer.size();
    
    // 显示当前步骤的提示信息
    cout << "\n==== 提示信息 ====" << endl;
    
    if(getIsRelative()){
        // 如果要猜B的相对航迹
        if (currentStep < initializer->getRelativeB().size()){
            cout << "B的相对航迹第" << (currentStep + 1) << "步为: (" 
                 << initializer->getRelativeB()[currentStep].x << "," 
                 << initializer->getRelativeB()[currentStep].y << ")" << endl;
        } 
        else{
            cout << "已经没有更多步骤可以提示了！" << endl;
            return;
        }
    } 
    else{
        if(currentStep < initializer->getShipB().getTrajectory().size()) {
            cout << "B的实际航迹第" << (currentStep + 1) << "步为: (" 
                 << initializer->getShipB().getTrajectory()[currentStep].x << "," 
                 << initializer->getShipB().getTrajectory()[currentStep].y << ")" << endl;
        } 
        else{
            cout << "已经没有更多步骤可以提示了！" << endl;
            return;
        }
    }
    // 应用时间惩罚
    applyHintTimePenalty();
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

bool BaseGameModeEngine::startBaseGameMode(AbstractInitializer* initializer){
    // 显示游戏信息
    cout << "网格大小: " << initializer->getGridSize() << "x" << initializer->getGridSize() << endl;
    cout << "航迹步数: " << initializer->getSteps() << endl;

    // 执行游戏
    return getCurrentBaseMode()->playBaseGameMode(initializer);
}

chrono::seconds BaseGameModeEngine::getTimeCost(){ return currentBaseGameMode -> getTimeCost(); }
