#include "MatchManager.h"
#include "Game.h"
#include <unordered_set>

MatchManager& MatchManager::getInstance() {
    static MatchManager instance;
    return instance;
}

void MatchManager::addPlayer(Connection* connection) {
    if (connection == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(mtx);
    matchPool.insert(connection);
}

void MatchManager::removePlayer(Connection* connection) {
    if (connection == nullptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(mtx);
    matchPool.erase(connection);
}

bool MatchManager::matching(Connection* connection) {
    std::lock_guard<std::mutex> lock(mtx);
    if (matchPool.find(connection) == matchPool.end()) {
        return false;
    }
    if (matchPool.size() < 2) {
        return false;
    }
    auto player1 = connection;
    matchPool.erase(player1);
    auto player2 = *matchPool.begin();
    matchPool.erase(player2);

    // 创建游戏实例
    Game* game = new Game(player1, player2);

    return true;
}