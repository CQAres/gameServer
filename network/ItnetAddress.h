#pragma once
#include <cstdint>
#include <arpa/inet.h>

// @brief 网络地址类,包含IPv4地址与端口信息
class ItnetAddress
{
private:
    struct sockaddr_in addr;  // IPv4地址结构体，包含地址族、端口、IP地址等信息 
public:
    ItnetAddress();

    /**
     * @brief 初始化IPv4地址结构体
     * @param IPv4地址
     * @param 端口
     */
    ItnetAddress(const char* _ip, uint16_t _port);

    ~ItnetAddress();

    /*
    * @brief 修改IPv4 socket address
    */
    void setInetAddr(sockaddr_in _addr);

    /*
    * @brief 获取IPv4 socket address
    * @return sockaddr_in
    */
    sockaddr_in getAddr();

    /*
    * @brief 获取ip地址
    * @return ip地址
    */
    char* getIp();

    /*
    * @brief 获取端口
    * @return 端口
    */
    uint16_t getPort();
};

