#pragma once
#include <functional>

class EventLoop;
class Socket;
class Channel;

// 主服务器，接收连接类
class Acceptor
{
private:
    EventLoop *loop; // 事件循环
    Socket *sock; // 监听socket
    Channel *acceptChannel; // 接受连接的channel
    std::function<void(Socket*)> newConnectionCallback; // 新连接回调函数
public:
    /*
    * @brief 构造函数，初始化一个网络连接接收器（Acceptor），负责监听指定地址和端口上的连接请求
    * @param _loop 事件循环对象
    */
    Acceptor(EventLoop *_loop);

    /*
    * @brief 析构函数，释放Acceptor对象
    */
    ~Acceptor();

    /*
    * @brief 接收新连接
    */
    void acceptConnection();

    /*
    * @brief 设置新连接回调函数
    */
    void setNewConnectionCallback(std::function<void(Socket*)>);
};