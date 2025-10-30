#include "MatchManager.h"
#include "Game.h"

MatchManager::MatchManager() : running(false) {}

MatchManager::~MatchManager() {
    stop();
}

MatchManager& MatchManager::getInstance() {
    static MatchManager instance;
    return instance;
}

void MatchManager::addPlayer(std::shared_ptr<Connection> connection) {
    if (!connection) {
        return;
    }
    std::lock_guard<std::mutex> lock(mtx);
    matchPool.insert(connection);
}

void MatchManager::removePlayer(std::shared_ptr<Connection> connection) {
    if (!connection) {
        return;
    }
    std::lock_guard<std::mutex> lock(mtx);
    matchPool.erase(connection); // 直接用 shared_ptr 删除对应 weak_ptr
}

int MatchManager::getMatchPoolSize() {
    std::lock_guard<std::mutex> lock(mtx);
    return matchPool.size();
}

void MatchManager::start() {
    running = true;
    matchThread = std::thread(&MatchManager::matchLoop, this);
    deleteThread = std::thread(&MatchManager::deleteLoop, this);
}

void MatchManager::stop() {
    running = false;
    if (matchThread.joinable()) {
        matchThread.join();
    }
    if (deleteThread.joinable()) {
        deleteThread.join();
    }
}

void MatchManager::deleteLoop() {
    const int timeoutSec = 15;

    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (matchPool.empty()) continue;

        std::lock_guard<std::mutex> lock(mtx);
        for (auto it = matchPool.begin(); it != matchPool.end(); ) {
            // 无效
            if (it->expired()) {
                it = matchPool.erase(it);
                continue;
            }
            auto connection = it->lock();
            // 超时
            if (connection && connection->isMatchTimedOut(timeoutSec)) {
                printf("Matching timeout for user fd %d\n", connection->getSockfd());
                it = matchPool.erase(it);
                
                // 通知连接主线程
                connection->runInLoop([connection]() {
                        connection->setState(ConnState::LOBBY);
                        connection->send(connection->getSockfd(), "Matching timeout. You have exited the Matching.\n");
                });
                continue;
            }
            ++it;
        }
    }
}

void MatchManager::matchLoop() {
    while (running) {
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::shared_ptr<Connection> player1, player2;

            {
                std::lock_guard<std::mutex> lock(mtx);
                if (matchPool.size() < 2) continue;

                auto it = matchPool.begin();
                auto player1 = it->lock();
                matchPool.erase(it);

                it = matchPool.begin();
                auto player2 = it->lock();
                matchPool.erase(it);
            }

            if (player1 && player2) {

                // 通知玩家匹配成功（需要放到对应连接的主线程去执行）
                player1->getLoop()->queueInLoop([player1](){
                    player1->send(player1->getSockfd(), "Match success!\n");
                    player1->setState(ConnState::IN_GAME);
                });
                player2->getLoop()->queueInLoop([player2](){
                    player2->send(player2->getSockfd(), "Match success!\n");
                    player2->setState(ConnState::IN_GAME);
                });
                printf("Matched fd %d <-> fd %d\n", player1->getSockfd(), player2->getSockfd());
                // 创建游戏实例
                Game* game = new Game(player1, player2);
            }
        }
    }
}