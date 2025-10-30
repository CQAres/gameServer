#include "Connection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "MatchManager.h"
#include "network/Socket.h"
#include "util/util.h"
#include "util/Buffer.h"
#include <unistd.h>
#include <string.h>
#include <vector>

Connection::Connection(EventLoop *_loop, Socket *_sock)
: loop(_loop), sock(_sock), channel(nullptr), readBuffer(nullptr), state(ConnState::LOBBY)
{
    // channel = new Channel(loop, sock->getFd());
    channel = std::make_unique<Channel>(loop, sock->getFd());
    channel->enableRead();
    channel->useET();

    // 捕获 shared_ptr，确保回调期间对象不会被析构
    std::shared_ptr<Connection> self = shared_from_this();
    channel->setReadCallback([self, fd=sock->getFd()](){
        self->echo(fd);
    });
    // std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    // channel->setReadCallback(cb);
    
    readBuffer = std::make_unique<Buffer>();
}

Connection::~Connection(){
    // 智能指针自动释放
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> _cb){
    deleteConnectionCallback = std::move(_cb);
}


void Connection::echo(int sockfd){
    std::vector<char> buf(128); //动态缓冲区
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
    if (readBuffer->size() != 0) {
        sendEco(sockfd);
        readBuffer->clear(); 
    }
    // 检查是否主动断开连接
    if (state == ConnState::EXITED) {
        printf("client fd %d disconnected\n", sockfd);
        deleteConnectionCallback(sockfd);           
    }
}

void Connection::sendEco(int sockfd){
    if (sockfd == -1) return;
    const char *data = readBuffer->c_str();
    size_t size = readBuffer->size();
    size_t sent = 0;
    while (sent < size) {
        ssize_t n = write(sockfd, data + sent, size - sent);
        if (n == -1 && errno == EAGAIN) break;
        sent += n;
    }
}

void Connection::send(int sockfd, const std::string &message) {
    if (sockfd == -1) return;
    const char *data = message.c_str();
    size_t size = message.size();
    size_t sent = 0;
    while (sent < size) {
        ssize_t n = write(sockfd, data + sent, size - sent);
        if (n == -1 && errno == EAGAIN) break;
        sent += n;
    }
}

int Connection::getSockfd() {
    return this->sock->getFd();
}

ConnState Connection::getState() {
    return this->state.load();
}

void Connection::setState(ConnState _state) {
    this->state.store(_state);
}

EventLoop* Connection::getLoop() {
    return this->loop;
}

void Connection::startMatchTimer() {
    matchStartTime = std::chrono::steady_clock::now();
}

bool Connection::isMatchTimedOut(int timeoutSeconds) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - matchStartTime).count();
    return elapsedTime >= timeoutSeconds;
}

void Connection::handleMessage(const std::string &_message, int sockfd) {
    if (_message.empty()) return;

    if (_message == "exit") {
        if (this->state == ConnState::MATCHING) {
            auto self = shared_from_this();
            MatchManager::getInstance().removePlayer(self);
            this->send(sockfd, "You have disconnected from the server.\n");
            setState(ConnState::EXITED);
            return;
        }

        if (this->state == ConnState::IN_GAME) {
            this->send(sockfd, "You have disconnected from the server.\n");
            setState(ConnState::EXITED);
            return;
        }

        if (this->state == ConnState::LOBBY) {
            this->send(sockfd, "You have disconnected from the server.\n");
            setState(ConnState::EXITED);
            return;
        }
    }

    if (_message == "quit") {
        if (this->state != ConnState::MATCHING) return;
        // 用户退出匹配池
        setState(ConnState::LOBBY);
        auto self = shared_from_this();
        MatchManager::getInstance().removePlayer(self);
        this->send(sockfd, "You have exited the matching queue.\n");
    }

    if (_message == "start match" && this->state == ConnState::LOBBY) {
        // 用户加入匹配池
        setState(ConnState::MATCHING);
        auto self = shared_from_this();
        self->startMatchTimer(); // 计时
        MatchManager::getInstance().addPlayer(self);
        printf("User fd %d start matching\n", sockfd);
    }
}

void Connection::runInLoop(std::function<void()> cb) {
    if (loop->isInLoopThread()) {
        cb(); // 在 loop 自己线程直接执行
    } else {
        loop->queueInLoop(std::move(cb)); // 跨线程调度到 loop 对应的主线程去执行
    }
}