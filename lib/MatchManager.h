#pragma once
#include "Connection.h"
#include "EventLoop.h"
#include <vector>
#include <mutex>
#include <functional>
#include <unordered_set>
#include <atomic>
#include <thread>

/**
* 匹配池中存放的是连接对象的弱引用，也就是unordered_set<std::weak_ptr<Connection>>
* 避免循环引用的问题，同时能确保连接对象在外部释放时匹配池中的对象同时被释放;
* 但weak_ptr 不直接等于 shared_ptr，二者类型不同
* 需要自定义hash和equal函数
*/
struct WeakPtrHash {
    size_t operator()(const std::weak_ptr<Connection>& w) const {
        if (auto sp = w.lock()) {
            return std::hash<Connection*>()(sp.get());
        }
        return 0;
    }
    size_t operator()(const std::shared_ptr<Connection>& sp) const {
        return std::hash<Connection*>()(sp.get());
    }
};

struct WeakPtrEqual {
    bool operator()(const std::weak_ptr<Connection>& w1, const std::weak_ptr<Connection>& w2) const {
        return !w1.owner_before(w2) && !w2.owner_before(w1);
    }

    bool operator()(const std::weak_ptr<Connection>& w, const std::shared_ptr<Connection>& s) const {
        return !w.owner_before(s) && !s.owner_before(w);
    }

    bool operator()(const std::shared_ptr<Connection>& s, const std::weak_ptr<Connection>& w) const {
        return !w.owner_before(s) && !s.owner_before(w);
    }
};

class MatchManager {
private:
    MatchManager() = default; // 构造函数私有化（单例）
    ~MatchManager();

    // std::unordered_set<Connection*> matchPool; // 匹配池
    std::unordered_set<
        std::weak_ptr<Connection>,
        WeakPtrHash,
        WeakPtrEqual
    > matchPool; // 匹配池，存放连接的弱引用

    std::mutex mtx;
    std::thread matchThread; // 匹配线程
    std::thread deleteThread; // 匹配池清理线程
    std::atomic<bool> running; // 匹配线程运行状态

    void matchLoop(); // 匹配循环
public:
    static MatchManager& getInstance();
    
    /**
    * @brief 将连接添加到匹配池中
    * @param connection 指向玩家连接的指针
    */
    void addPlayer(std::shared_ptr<Connection> connection);

    /**
    * @brief 从匹配池中删除连接
    * @param connection 指向玩家连接的指针
    */
    void removePlayer(std::shared_ptr<Connection> connection);

    /**
    * @brief 获取匹配池的大小
    * @return 匹配池的大小
    */
    int getMatchPoolSize();

    /**
    * @brief 启动匹配线程
    */
    void start();

    /**
    * @brief 停止匹配线程
    */
    void stop();

    /**
    * @brief 清理函数
    * 每隔1s检查匹配池中是否有过期/无效连接，有则删除
    */
    void deleteLoop();
};