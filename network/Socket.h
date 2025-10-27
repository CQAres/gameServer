#pragma once
#include "ItnetAddress.h"

// socket类
class Socket
{
private:
    int fd;
public:
    Socket();

    /*
    * @brief 使用已有的文件描述符构造Socket对象
    */
    Socket(int _fd);
    ~Socket();

    /*
    * @brief 绑定socket到指定的网络地址
    * @param ItnetAddress信息
    */
    void bind(ItnetAddress*);

    /*
    * @brief 监听端口（服务器使用）
    */
    void listen();

    /*
    * @brief 接收连接（服务器使用）
    * @param 客户端的ItnetAddress信息
    * @return 新连接的客户端socket文件描述符
    */
    int accept(ItnetAddress*);

    /*
    * @brief 连接服务器（客户端使用）
    * @param 服务器的ItnetAddress信息
    */
    void connect(ItnetAddress*);

    /*
    * @brief 设置为非阻塞模式
    * @brief 非阻塞模式下，套接字的 I/O 操作不会阻塞程序的执行，而是立即返回
    */
    void setnonblocking();

    /*
    * @brief 获取文件描述符fd
    */
    int getFd();
};

