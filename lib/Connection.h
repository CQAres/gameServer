#pragma once
#include <functional>
#include <string>
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
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback; // 删除连接回调函数
    Buffer *readBuffer; // 读缓冲区

    ConnState state; // 连接状态
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

    void handleMessage(std::string _message, int sockfd);

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

    void send(int sockfd, std::string message);

    int getSockfd();

    void setState(ConnState _state);

};