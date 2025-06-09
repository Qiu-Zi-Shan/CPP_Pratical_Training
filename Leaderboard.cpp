#include "Mental_Trajectory.h"

const string PlayerManager::PLAYER_DATA_FILE = "C:\\Cpl\\CPP_Pratical_Training\\player_data.txt";
const string LeaderboardManager::CHALLENGE_LEADERBOARD_FILE = "C:\\Cpl\\CPP_Pratical_Training\\challenge_leaderboard.txt";
const string LeaderboardManager::STAGE_LEADERBOARD_FILE = "C:\\Cpl\\CPP_Pratical_Training\\stage_leaderboard.txt";
const string LeaderboardManager::LEVEL_TIME_FILE = "C:\\Cpl\\CPP_Pratical_Training\\level_time";
const string LeaderboardManager::FUN_LEADERBOARD_FILE = "C:\\Cpl\\CPP_Pratical_Training\\fun_leaderboard.txt";

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
                 << fixed << setprecision(1) << get<3>(players[i]) * 100 << "%" << endl;
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

void LeaderboardManager::updateFunLeaderboard(const Player& player){
    // 读取现有排行榜数据
    vector<tuple<string, int, double>> leaderboardData;
    
    // 从文件读取现有数据
    ifstream inFile(FUN_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int score;
            double rate;
            ss >> name >> score >> rate;
            
            // 跳过与当前玩家同名的记录
            if(name == player.getUsername()) continue;
            
            leaderboardData.push_back(make_tuple(name, score, rate));
        }
        inFile.close();
    }
    
    // 添加当前玩家数据
    leaderboardData.push_back(make_tuple(player.getUsername(), player.getFunScore(), player.getSuccessRate()));
    
    // 按照分数排序，相同分数按成功率排序
    sort(leaderboardData.begin(), leaderboardData.end(),
         [](const auto& a, const auto& b) {
             // 首先按积分排序
             if (get<1>(a) != get<1>(b)) {
                 return get<1>(a) > get<1>(b);
             }
             // 其次按成功率排序
             return get<2>(a) > get<2>(b);
         });
    
    // 保存排行榜
    ofstream outFile(FUN_LEADERBOARD_FILE);
    if(outFile.is_open()){
        for (const auto& entry : leaderboardData) {
            outFile << get<0>(entry) << " " << get<1>(entry) << " " << get<2>(entry) << endl;
        }
        outFile.close();
    } else {
        cout << "无法打开排行榜文件，保存失败" << endl;
    }
}
void LeaderboardManager::displayFunLeaderboard(){
    // 读取排行榜数据
    vector<tuple<string, int, double>> leaderboardData;
    
    ifstream inFile(FUN_LEADERBOARD_FILE);
    if(inFile.is_open()){
        string line;
        while(getline(inFile, line)){
            stringstream ss(line);
            string name;
            int score;
            double rate;
            ss >> name >> score >> rate;
            
            leaderboardData.push_back(make_tuple(name, score, rate));
        }
        inFile.close();
        
        // 显示排行榜
        cout << "\n===== 娱乐模式排行榜 =====" << endl;
        cout << "排名\t用户名\t积分\t成功率" << endl;
        
        for(int i = 0; i < leaderboardData.size(); ++i){
            cout << i + 1 << "\t"
                 << get<0>(leaderboardData[i]) << "\t"
                 << get<1>(leaderboardData[i]) << "\t"
                 << fixed << setprecision(1) << get<2>(leaderboardData[i]) * 100 << "%" << endl;
        }
        
        if(leaderboardData.empty()){
            cout << "暂无玩家数据" << endl;
        }
    } 
    else{
        cout << "无法打开排行榜文件。" << endl;
    }
}


