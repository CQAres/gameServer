#pragma once
#include <functional>
class EventLoop;
class Socket;
class Channel;
class Buffer;

// 连接类，封装每个客户端的连接
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback; // 删除连接回调函数
    Buffer *readBuffer; // 读缓冲区
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

    /*
    * @brief 设置删除连接回调函数
    * @param _callback 删除连接回调函数
    */
    void setDeleteConnectionCallback(std::function<void(int)>);

    /*
    * @brief 把读缓冲区的数据发送给客户端socket
    * @param sockfd 套接字描述符
    */
    void send(int sockfd);
};