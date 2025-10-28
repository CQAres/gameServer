#include "Connection.h"
#include "Channel.h"
#include "MatchManager.h"
#include "network/Socket.h"
#include "util/util.h"
#include "util/Buffer.h"
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>

Connection::Connection(EventLoop *_loop, Socket *_sock)
: loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr), state(ConnState::LOBBY)
{
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();
    channel->useET();
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setReadCallback(cb);
    readBuffer = new Buffer();
}

Connection::~Connection(){
    delete channel;
    delete sock;
    delete readBuffer;
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb){
    deleteConnectionCallback = _cb;
}


void Connection::echo(int sockfd){
    std::vector<char> buf(1024); //动态缓冲区
    while(true){
        //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        ssize_t bytes_read = read(sockfd, buf.data(), buf.size());
        if(bytes_read > 0){
            readBuffer->append(buf.data(), bytes_read);
            handleMessage(readBuffer->c_str(), sockfd);
        } else if(bytes_read == 0){  
            //客户端主动断开连接
            printf("client fd %d disconnected\n", sockfd);
            deleteConnectionCallback(sockfd);           
            return;
        } else {
            // 异常处理
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                // 非阻塞IO，这个条件表示数据全部读取完毕
                printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                break;
            } else if(errno == EINTR){
                 //系统调用被中断，继续读取
                printf("continue reading\n");
                continue;
            } else {
                // 其他错误（如连接重置）
                printf("Connection reset by peer\n");
                deleteConnectionCallback(sockfd);         
                break;
            }
        }
    }
    // 回显并清空缓冲区
    if (!readBuffer->size() == 0) {
        sendEco(sockfd);
        readBuffer->clear(); 
    }
    // 检查是否主动断开连接
    if (state == ConnState::EXITED) {
        printf("client fd %d disconnected\n", sockfd);
        deleteConnectionCallback(sockfd);           
        return;
    }
}

void Connection::sendEco(int sockfd){
    char buf[readBuffer->size()];
    strcpy(buf, readBuffer->c_str());
    int  data_size = readBuffer->size(); 
    int data_left = data_size;
    while (data_left > 0)
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }
}

void Connection::send(int sockfd, std::string message) {
    int data_size = message.size();
    int data_left = data_size;
    const char* buf = message.c_str();
    while (data_left > 0)
    { 
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left); 
        if (bytes_write == -1 && errno == EAGAIN) { 
            break;
        }
        data_left -= bytes_write; 
    }
}

int Connection::getSockfd() {
    return this->sock->getFd();
}

void Connection::setState(ConnState _state) {
    this->state = _state;
    return;
}

void Connection::handleMessage(std::string _message, int sockfd) {
    if (_message == "start_match" && this->state == ConnState::LOBBY) {
        // 用户加入匹配池
        this->state = ConnState::MATCHING;
        MatchManager::getInstance().addPlayer(this);
        printf("User fd %d start matching\n", sockfd);

        // 启动匹配线程进行匹配
        std::thread([this, sockfd]() {
            auto startTime = std::chrono::steady_clock::now();

            while (this->state == ConnState::MATCHING) {
                // 检查是否超时
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
                if (elapsedTime >= 15) {
                    // 匹配超时，移除用户并退出线程
                    this->state = ConnState::LOBBY;
                    MatchManager::getInstance().removePlayer(this);
                    printf("Matching timeout for user fd %d\n", sockfd);
                    break;
                }
                // 调用匹配逻辑
                bool matched = MatchManager::getInstance().matching(this);
                if (matched) {
                    break;
                }

                this->send(sockfd,"Matching..., elapsedTime: " + std::to_string(elapsedTime) + ".\n");
                // 休眠1秒后继续匹配
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }).detach();
    } 
    if (_message == "quit") {
        if (this->state == ConnState::LOBBY) {
            this->state = ConnState::EXITED;
            return;
        }
        if (this->state == ConnState::MATCHING) {
            // 用户退出匹配池
            this->state = ConnState::LOBBY;
            MatchManager::getInstance().removePlayer(this);
            this->send(sockfd, "You have exited the matching queue.\n");
        }
    }
}