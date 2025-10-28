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

    // 修改两位玩家的状态
    player1->setState(ConnState::IN_GAME);
    player2->setState(ConnState::IN_GAME);

    int sockfd1 = player1->getSockfd();
    int sockfd2 = player2->getSockfd();
    player1->send(sockfd1, "Match found! Game started.\n");
    player2->send(sockfd2, "Match found! Game started.\n");
    printf("User fd %d matched successfully\n", player1->getSockfd());
    printf("User fd %d matched successfully\n", player2->getSockfd());

    // 创建游戏实例
    Game* game = new Game(player1, player2);

    return true;
}