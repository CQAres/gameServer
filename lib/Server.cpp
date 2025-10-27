#include "Server.h"
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "network/Socket.h"
#include "util/ThreadPool.h"
#include <unistd.h>
#include <functional>

Server::Server(EventLoop *_loop) 
: mainReactor(_loop), acceptor(nullptr)
{ 
    // 初始化主Reactor和acceptor
    // 设置新连接回调函数，用于处理新连接的Socket
    // 创建线程池，线程数量与硬件支持的并发线程数相同
    // 为每个线程创建子Reactors，并将子Reactors的事件循环绑定到线程池中
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    thpool = new ThreadPool(size);
    for(int i = 0; i < size; ++i){
        subReactors.push_back(new EventLoop());
    }

    for(int i = 0; i < size; ++i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        thpool->add(sub_loop);
    }
}

Server::~Server(){
    delete acceptor;
    delete thpool;
}


void Server::newConnection(Socket *sock){
    if(sock->getFd() == -1) {
        return;
    }
    int random = sock->getFd() % subReactors.size();
    Connection *conn = new Connection(subReactors[random], sock);
    std::function<void(int)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[sock->getFd()] = conn;
}

void Server::deleteConnection(int sockfd){
    if(sockfd == -1) {
        return;
    }
    if(connections.find(sockfd) == connections.end()) {
        return;
    }
    Connection *conn = connections[sockfd];
    connections.erase(sockfd);
    // close(sockfd);
    delete conn;
}