#pragma once
#include <functional>
#include <string>
#include <memory>
#include <atomic>

class EventLoop;
class Socket;
class Channel;
class Buffer;

// 连接状态
enum class ConnState {
    EXITED,         // 已断开连接
    LOBBY,          // 在大厅
    MATCHING,       // 匹配中
    IN_GAME         // 已进入游戏
};

// 连接类，封装每个客户端的连接
class Connection : public std::enable_shared_from_this<Connection> // 在内部可以安全获取自己的 shared_ptr
{
private:
    EventLoop *loop;
    std::unique_ptr<Socket> sock;
    std::unique_ptr<Channel> channel;
    std::unique_ptr<Buffer> readBuffer; // 读缓冲区
    std::function<void(int)> deleteConnectionCallback; // 删除连接回调函数

    std::atomic<ConnState> state; // 连接状态
    std::chrono::steady_clock::time_point matchStartTime; // 开始匹配时间点
public:
    /*
    * @brief 构造函数，初始化连接对象
    * @param _loop 事件循环指针
    * @param _sock 连接的Socket指针
    */
    Connection(EventLoop *_loop, Socket *_sock);

    /*
    * @brief 释放资源
    */
    ~Connection();
    
    /*
    * @brief 从客户端socket读取数据并处理
    * @param sockfd 套接字描述符
    */
    void echo(int sockfd);

    void handleMessage(const std::string &_message, int sockfd);

    /*
    * @brief 设置删除连接回调函数
    * @param _callback 删除连接回调函数
    */
    void setDeleteConnectionCallback(std::function<void(int)>);

    /*
    * @brief 把读缓冲区的数据发回给客户端socket（回显）
    * @param sockfd 套接字描述符
    */
    void sendEco(int sockfd);

    /**
    * @brief 发送消息给客户端
    * @param sockfd 套接字描述符
    * @param message 要发送的消息内容
    */
    void send(int sockfd, const std::string &message);

    /**
    * @brief 获取连接状态
    * @return 连接状态
    */
    ConnState getState();

    /**
    * @brief 获取连接的socket文件描述符
    * @return socket文件描述符
    */
    int getSockfd();

    /**
    * @brief 设置连接状态
    * @param _state 要设置的状态
    */
    void setState(ConnState _state);

    EventLoop* getLoop();

    /**
    * @brief 在事件循环线程中执行回调函数
    * 如果当前线程是事件循环线程，则直接执行回调函数；
    * 否则将回调函数调度到事件循环线程中执行
    * @param cb 需要执行的回调函数。
    */
   void runInLoop(std::function<void()> cb);

   void startMatchTimer();

   bool isMatchTimedOut(int timeoutSeconds);
};