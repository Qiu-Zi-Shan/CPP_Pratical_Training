#include "Mental_Trajectory.h"

FunModePlayer::FunModePlayer() : currentCell(nullptr), startCell(nullptr), hasFlag(false){}
void FunModePlayer::setStartCell(HexCell* cell){
    this->startCell = cell;
    currentCell = cell;
    moveHistory.clear();  // 清空移动历史
    moveHistory.push_back(cell);
}
void FunModePlayer::moveToCell(HexCell* targetCell){
    // 移动到新位置
    currentCell = targetCell;
    moveHistory.push_back(currentCell);
    // 检查是否到达旗帜
    if(currentCell->getIsFlag()){ 
        hasFlag = true;
    }
}
void FunModePlayer::moveBackward(){
    // 历史记录不为空
    if(!moveHistory.empty()){
        // 如果历史记录只有起点，则退回起点
        if(moveHistory.size() == 1){ 
            currentCell = startCell; 
            cout << "无法后退, 当前位置为起点" << endl;
        }
        else{
            moveHistory.pop_back();
            currentCell = moveHistory.back();
        }
        // 如果刚才在旗帜位置，现在离开，则重置hasFlag
        if(hasFlag && !currentCell->getIsFlag()){ hasFlag = false; }
    }
}

// AIPlayer 实现
AIPlayer::AIPlayer(int gameDifficulty) : FunModePlayer(), aiDifficulty(gameDifficulty){}
bool AIPlayer::playGame(int baseGameDifficulty){
    // 游戏难度越高，ai成功率越高；游戏难度越低，ai成功率越低
    double baseSuccessRate = 0.8; // 基础成功率80%

    // 根据游戏难度调整（1-3）
    double aiBonus = (aiDifficulty - 2) * 0.15; // 难度1: -0.15, 难度2: 0, 难度3: +0.15 

    // 根据距离旗帜的距离调整 - 越近惩罚越大
    double distancePenalty = 0;
    switch(baseGameDifficulty){
        case 1: distancePenalty = 0; break;
        case 2: distancePenalty = 0.05; break;
        case 3: distancePenalty = 0.1; break;
        case 4: distancePenalty = 0.15; break;
        case 5: distancePenalty = 0.2; break;
    }
    
    // 计算最终成功率
    double finalSuccessRate = baseSuccessRate + aiBonus - distancePenalty;

    // 随机决定是否成功
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    
    bool result = dis(gen) < finalSuccessRate;
    
    // 显示结果
    cout << "#AI挑战难度为 " << baseGameDifficulty << " 的游戏";

    
    return result;
}
HexCell* AIPlayer::chooseStartCell(const vector<HexCell*>& startCells, HexCell* flagCell, int mapSize){
    HexCell* bestStart = nullptr;
    int index = -1;
    // 根据AI难度选择策略
    // 简单AI: 随机选择未被选择的起点
    if(aiDifficulty == 1){
        random_device rd;
        mt19937 gen(rd());
        int randomIndex = 0;
        while(true){
            uniform_int_distribution<> dis(0, startCells.size() - 1);
            index = dis(gen);
            if(!startCells[index]->getHasBeenChoosen()) break;
        }
        setBestPath(findBestPath(startCells[index], flagCell).first);
        bestStart = startCells[index];
    } 
    // 较高级AI:各起点最优路径总难度最低的起点
    else if(aiDifficulty == 2){
        int lowestTotalDifficulty = INT_MAX;
        
        // 评估每个起点到旗帜的路径
        for(auto* start : startCells){
            // 跳过已被选择的起点
            if(start->getHasBeenChoosen()) continue;
            
            // 计算从该起点到旗帜的最佳路径
            pair<vector<HexCell*>, int> pathInfo = findBestPath(start, flagCell);
            
            // 如果找到路径，检查总难度
            if(!pathInfo.first.empty()){
                int totalDiff = pathInfo.second;
                
                // 如果总难度更低，更新最佳起点
                if(totalDiff < lowestTotalDifficulty){
                    lowestTotalDifficulty = totalDiff;
                    bestStart = start;
                }
            }
        }
    }
    // 高级ai:各起点到旗帜距离最短的起点
    else{ 
        int shortestDistance = INT_MAX;

        // 评估每个起点到旗帜的距离
        for(auto* start : startCells){
            // 跳过已被选择的起点
            if(start->getHasBeenChoosen()) continue;
            
            // 获取到旗帜的距离
            int distance = HexMap::calculateDistance(start, flagCell, mapSize);
            
            // 如果距离更短，更新最佳起点
            if(distance < shortestDistance){
                shortestDistance = distance;
                bestStart = start;
            }
        }
    }
    
    // 如果没找到最佳起点，随机选择一个未被选择的
    if(!bestStart){
        while(true){
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(0, startCells.size() - 1);
            index = dis(gen);
            if(!startCells[index]->getHasBeenChoosen()) break;
        }
        bestStart = startCells[index];
    }
    // 设置最佳路径
    setBestPath(findBestPath(bestStart, flagCell).first);

    // 如果索引为-1，根据找到的最佳起点，找到其索引,便于输出选择的额起点
    if(index < 0){
        for(int i = 0; i < startCells.size(); i++){
            if(startCells[i] == bestStart){
                index = i;
                break;
            }
        }
    }
    bestStart->setHasBeenChoosen(true);
    cout << "\n#AI选择了起点 " << static_cast<char>(index + 65) << endl;
    return bestStart;
}
HexCell* AIPlayer::chooseNextMove(){
    // 在 bestPath 中找到当前单元格的位置
    auto it = find(bestPath.begin(), bestPath.end(), currentCell);

    // 如果找到了当前单元格，并且它不是路径的最后一个节点
    if(it != bestPath.end() && next(it) != bestPath.end()){
        // 返回路径中的下一个单元格作为移动目标
        HexCell* nextCell = *next(it);
        return nextCell;
    }

    // 如果因为某些原因无法在路径上找到下一步，则随机选择一个未访问过的邻居
    for(auto* neighbor : currentCell->getNeighbors()){
        if(neighbor->getIsMapCell() && find(moveHistory.begin(), moveHistory.end(), neighbor) == moveHistory.end()){
            return neighbor;
        }
    }

    return nullptr; // 没有可移动的地方
}
pair<vector<HexCell*>, int> AIPlayer::findBestPath(HexCell* startPoint, HexCell* flagCell){ 
    // 使用优先队列进行Dijkstra算法，按路径总难度排序
    priority_queue<pair<int, HexCell*>, vector<pair<int, HexCell*>>, greater<pair<int, HexCell*>>> pq; // pair<总难度, 单元格指针>
        
    // 记录到每个单元格的最小总难度
    map<HexCell*, int> totalDifficulty;
        
    // 记录路径
    map<HexCell*, HexCell*> parent;
        
    // 初始化
    totalDifficulty[startPoint] = startPoint->getDifficulty();
    pq.push({totalDifficulty[startPoint], startPoint});
    parent[startPoint] = nullptr;
        
    // Dijkstra算法
    while(!pq.empty()){
        auto [currDiff, currCell] = pq.top();
        pq.pop();
            
        // 如果已经找到旗帜，结束搜索
        if(currCell == flagCell) break;
            
        // 如果当前的总难度大于已知的最小总难度，跳过
        if(currDiff > totalDifficulty[currCell]) continue;
            
        // 检查所有邻居
        for(HexCell* neighbor : currCell->getNeighbors()){
            // 只考虑地图单元格
            if(neighbor->getIsMapCell()){
                // 计算通过当前单元格到达邻居的总难度
                int newDifficulty = totalDifficulty[currCell] + neighbor->getDifficulty();
                
                // 如果找到更低难度的路径，或者这个邻居还没有被访问过
                if(totalDifficulty.find(neighbor) == totalDifficulty.end() || 
                    newDifficulty < totalDifficulty[neighbor]){
                    totalDifficulty[neighbor] = newDifficulty;
                    pq.push({newDifficulty, neighbor});
                    parent[neighbor] = currCell;
                }
            }
        }
    }
        
    // 如果没有到达旗帜的路径，返回空路径
    if(parent.find(flagCell) == parent.end()) return {{}, 0};
    
    // 构建从起点到旗帜的路径
    vector<HexCell*> path;   
    HexCell* current = flagCell;
        
    // 从旗帜回溯到起点
    while(current){
        path.push_back(current);
        current = parent[current];
    }
        
    // 反转路径，使其从起点到旗帜
    reverse(path.begin(), path.end());

    return {path, totalDifficulty[flagCell]};
}
void AIPlayer::setBestPath(const vector<HexCell*>& path){ bestPath = path; }

FunPlayer::FunPlayer() : FunModePlayer(), funScore(0), funGames(0), funSuccessGames(0){}
bool FunPlayer::playGame(int baseGameDifficulty){
    // 使用基础游戏模式
    cout << "\n==== 玩家开始游戏挑战 ====" << endl;
    cout << "难度大小：" << baseGameDifficulty << endl;
    AbstractInitializer* initializer = new FunInitializer(static_cast<Player&>(*this), baseGameDifficulty);
    bool result = initializer->getBaseGameModeEngine().startBaseGameMode(initializer);
    addFunGame(result);
    delete initializer;  
    return result;
}
HexCell* FunPlayer::chooseStartCell(const vector<HexCell*>& startCells){
    cout << "\n===== 玩家选择起点 =====" << endl;
    for(int i = 0; i < startCells.size(); i++){
        if(!startCells[i]->getHasBeenChoosen()){
            cout << i + 1 << ". " << static_cast<char>(i + 65) << endl;
        }
        else{
            cout << i + 1 << ". " << static_cast<char>(i + 65) << " 已被选择" << endl;
        }
    }
    
    int choice;
    do{
        cout << "请输入起点编号(1-5): ";
        cin >> choice;
    } while (choice < 1 || choice > 5 || startCells[choice - 1]->getHasBeenChoosen());
    startCells[choice - 1]->setHasBeenChoosen(true);
    return startCells[choice - 1];
}
HexCell* FunPlayer::chooseNextMove(){
    vector<pair<string, HexCell*>> directions;
    auto [r, c] = currentCell->getPosition();
    
    for(auto neighbor : currentCell->getNeighbors()){
        if(neighbor->getIsMapCell()){
            auto [nr, nc] = neighbor->getPosition();
            string direction;
            
            // 根据相对位置确定方向
            if(r % 2 == 0){
                if(nr < r && nc <= c) direction = "左上方";
                else if(nr < r && nc > c) direction = "右上方";
                else if(nr > r && nc <= c) direction = "左下方";
                else if(nr > r && nc > c) direction = "右下方";
                else if(nr == r && nc < c) direction = "左方";
                else if(nr == r && nc > c) direction = "右方";
            } 
            else{
                if(nr < r && nc < c) direction = "左上方";
                else if(nr < r && nc >= c) direction = "右上方";
                else if(nr > r && nc < c) direction = "左下方";
                else if(nr > r && nc >= c) direction = "右下方";
                else if(nr == r && nc < c) direction = "左方";
                else if(nr == r && nc > c) direction = "右方";
            }
            
            directions.push_back({direction, neighbor});
        }
    }

    for(int i = 0; i < directions.size(); i++){
        cout << i + 1 << ". " << directions[i].first << "，难度: " << directions[i].second->getDifficulty() << endl;
    }
    
    int choice;
    do{
        cout << "选择下一步移动(输入编号): ";
        cin >> choice;
    } while (choice < 1 || choice > static_cast<int>(directions.size()));

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

    return directions[choice - 1].second;
}

void FunPlayer::setFunScore(int newScore) { funScore = newScore; }
void FunPlayer::addFunScore(int points) { funScore += points; }
void FunPlayer::addFunGame(bool success){
    funGames++;
    if(success){ funSuccessGames++; }
}
void FunPlayer::setTotalFunGames(int totalGames) { funGames = totalGames; }
void FunPlayer::setSuccessFunGames(int successGames) { funSuccessGames = successGames; }

double FunPlayer::getSuccessRate() const {
    if(funGames == 0){ return 0.0; }
    return static_cast<double>(funSuccessGames) / funGames;
}

ChallengePlayer::ChallengePlayer() : challengeScore(0), challengeGames(0), challengeSuccessGames(0), rank(PlayerRank::BRONZE) {}
void ChallengePlayer::setChallengeScore(int newScore) { challengeScore = newScore; }
void ChallengePlayer::addChallengeScore(int points) { challengeScore += points; }
void ChallengePlayer::reduceChallengeScore(int points) { challengeScore -= points; }
void ChallengePlayer::setTotalChallengeGames(int totalGames) { challengeGames = totalGames; }
void ChallengePlayer::setSuccessChallengeGames(int successGames) { challengeSuccessGames = successGames; }
void ChallengePlayer::addChallengeGame(bool success){ 
    challengeGames++;
    if(success) challengeSuccessGames++;
}
void ChallengePlayer::calculateRank(){
    if(challengeScore < 50) rank = PlayerRank::BRONZE;
    else if(challengeScore < 150) rank = PlayerRank::SILVER;
    else if(challengeScore < 300) rank = PlayerRank::GOLD;
    else if(challengeScore < 500) rank = PlayerRank::PLATINUM;
    else if(challengeScore < 800) rank = PlayerRank::DIAMOND;
    else rank = PlayerRank::MASTER;
}

double ChallengePlayer::getChallengeSuccessRate() const{ 
    return challengeGames == 0 ? 0 : static_cast<double>(challengeSuccessGames) / challengeGames;
}
string ChallengePlayer::getChallengeRankName() const {
    switch(rank){
        case PlayerRank::BRONZE: return "青铜";
        case PlayerRank::SILVER: return "白银";
        case PlayerRank::GOLD: return "黄金";
        case PlayerRank::PLATINUM: return "铂金";
        case PlayerRank::DIAMOND: return "钻石";
        case PlayerRank::MASTER: return "大师";
        default: return "未知";
    }
}

StagePlayer::StagePlayer() : currentStage(0){}
void StagePlayer::setCurrentStage(int stage) { currentStage = stage; }
void StagePlayer::setStageTime(int stage, int timeSeconds){
    // 如果没有该关卡记录或新时间更短，则更新
    if(!hasStageTimeRecord(stage) || timeSeconds < stageTimeRecords[stage]){
        stageTimeRecords[stage] = timeSeconds;
    }
}

int StagePlayer::getStageTime(int stage) const{
    auto it = stageTimeRecords.find(stage);
    if(it != stageTimeRecords.end()){
        return it->second;
    }
    return 0; // 返回0表示没有该关卡的记录
}
bool StagePlayer::hasStageTimeRecord(int stage) const{
    return stageTimeRecords.find(stage) != stageTimeRecords.end();
}

Player::Player() : FunPlayer(), ChallengePlayer(), StagePlayer(), username("no name"), password("no password"){}
Player::Player(const string& name, const string& pwd) : FunPlayer(), ChallengePlayer(), StagePlayer(), username(name), password(pwd){}
bool Player::checkPassword(const string& pwd) const{ return pwd == password; }

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
        outFile << username << " " << password << " 0 0 0 0 0 0 0" << endl;
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
            int funScore, funGames, funSuccess;
            ss >> name >> pwd >> challengeScore >> challengeTotal >> challengeSuccess 
               >> funScore >> funGames >> funSuccess >> currentStage;
            
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
                    player.setFunScore(funScore);
                    player.setTotalFunGames(funGames);
                    player.setSuccessFunGames(funSuccess);
                    
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
                    cout << "\n==== 当前玩家信息 ====" << endl;
                    cout << "挑战模式积分: " << challengeScore << endl;
                    cout << "挑战模式等级: " << player.getChallengeRankName() << endl;
                    cout << "挑战模式成功率: " << player.getChallengeSuccessRate() * 100 << "%" << endl;
                    cout << "闯关模式已闯关卡: " << player.getCurrentStage() << endl;
                    cout << "娱乐模式积分: " << funScore << endl;
                    cout << "娱乐模式成功率: " << player.getSuccessRate() * 100 << "%" << endl;
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
                       " " + to_string(player.getFunScore()) +
                       " " + to_string(player.getTotalFunGames()) +
                       " " + to_string(player.getSuccessFunGames()) +
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




