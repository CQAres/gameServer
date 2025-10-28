#pragma once
#include <vector>
#include <mutex>
#include <functional>
#include <unordered_set>

class Connection;

class MatchManager {
public:
    static MatchManager& getInstance();
    void addPlayer(Connection* connection);

    void removePlayer(Connection* connection);

    bool matching(Connection* connection);

private:
    MatchManager() = default;
    std::unordered_set<Connection*> matchPool; // 匹配池
    std::mutex mtx;
};