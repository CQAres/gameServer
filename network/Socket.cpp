#include "Socket.h"
#include "util/util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

Socket::Socket(): fd(-1) {
    this->fd = socket(AF_INET, SOCK_STREAM, 0);
    ifError(this->fd == -1, "socket create error");
}

Socket::Socket(int _fd) {
    this->fd = _fd;
    ifError(fd == -1, "socket create error");
}

Socket::~Socket() {
    if (this->fd != -1) {
        close(this->fd);
        this->fd = -1;
    }
}

int Socket::getFd() {
    return this->fd;
}

void Socket::bind(ItnetAddress* _addr) {
    if(_addr == nullptr) {
        ifError(true, "socket bind error: invalid address");
    }
    struct sockaddr_in addr = _addr->getAddr();
    ifError(::bind(this->fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket bind error");
}

void Socket::listen() {
    ifError(::listen(this->fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking() {
    fcntl(this->fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(ItnetAddress* _addr) {
    // for server socket
    int clnt_sockfd = -1;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr);
    if (fcntl(this->fd, F_GETFL) & O_NONBLOCK) {
        // 非阻塞模式
        // 在非阻塞模式下，如果当前没有连接请求，则循环等待
        while (true) {
            clnt_sockfd = ::accept(this->fd, (sockaddr*)&addr, &addr_len);
            if (clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
                // "no connection yet\n"
                continue;
            }
            else if (clnt_sockfd == -1) {
                ifError(true, "socket accept error");
            }
            else {
                break;
            }
        }
    }
    else {
        // 阻塞模式
        clnt_sockfd = ::accept(fd, (sockaddr*)&addr, &addr_len);
        ifError(clnt_sockfd == -1, "socket accept error");
    }

    _addr->setInetAddr(addr);
    return clnt_sockfd;
}

void Socket::connect(ItnetAddress* _addr) {
    // for client socket
    if(_addr == nullptr) {
        ifError(true, "client connect error: invalid server address");
    }

    struct sockaddr_in addr = _addr->getAddr();
    if (fcntl(fd, F_GETFL) & O_NONBLOCK) {
        // 非阻塞模式下，如果连接失败，则循环调用connect，直到连接成功
        while (true) {
            int ret = ::connect(fd, (sockaddr*)&addr, sizeof(addr));
            if (ret == 0) {
                break;
            }
            else if (ret == -1 && (errno == EINPROGRESS)) {
                continue;
                /* 
                    The socket is nonblocking and the connection cannot be
                  completed immediately.  (UNIX domain sockets failed with
                  EAGAIN instead.)  It is possible to select(2) or poll(2)
                  for completion by selecting the socket for writing.  After
                  select(2) indicates writability, use getsockopt(2) to read
                  the SO_ERROR option at level SOL_SOCKET to determine
                  whether connect() completed successfully (SO_ERROR is
                  zero) or unsuccessfully (SO_ERROR is one of the usual
                  error codes listed here, explaining the reason for the
                  failure).
                  
                */
            }
            else if (ret == -1) {
                ifError(true, "socket connect error");
            }
        }
    }
    else {
        ifError(::connect(this->fd, (sockaddr*)&addr, sizeof(addr)) == -1, "socket connect error");
    }
}