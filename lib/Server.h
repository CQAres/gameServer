#pragma once
#include <map>
#include <vector>
#include "MatchManager.h"

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *mainReactor; //主reactor
    Acceptor *acceptor;
    std::map<int, Connection*> connections; //连接池<sockfd, Connection*>
    std::vector<EventLoop*> subReactors; // 从reactor
    ThreadPool *thpool; // 线程池
public:
    /**
    @brief 初始化服务器实例。
    @param _loop 主事件循环指针，用于处理主reactor的事件。
    */
    Server(EventLoop* _loop);

    ~Server();
    
    /**
    * @brief 处理新连接请求，并将其分配到子reactor中
    * @param sock 指向新连接的Socket对象的指针
    * @note 如果Socket的fd有效，则会创建一个新的Connection对象，并将其分配到子reactor中
    */
    void newConnection(Socket *sock);

    /**
    * @brief 删除指定socket对应的连接
    * @param sockfd 要删除的连接的socket描述符
    * @note 调用此方法后，连接对象会被删除，但socket本身不会被关闭（注释中已说明）
    */
    void deleteConnection(int sockfd);

    int getConnectionCount();

    void monitorMatchPool();
};