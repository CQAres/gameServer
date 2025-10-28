#include "util/util.h"
#include "util/Buffer.h"
#include "network/Socket.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <thread>
#include <atomic>

using namespace std;

atomic<bool> isRunning(true);

// 接收服务器消息的线程函数
void receiveMessages(int sockfd, Buffer* readBuffer) {
    while (isRunning) {
        std::vector<char> buf(1024);
        bzero(buf.data(), buf.size());
        ssize_t read_bytes = read(sockfd, buf.data(), buf.size());
        if (read_bytes > 0) {
            readBuffer->append(buf.data(), read_bytes);
            printf("Message from server: %s\n", readBuffer->c_str());
            readBuffer->clear();
        } else if (read_bytes == 0) { // EOF，服务器断开连接
            printf("Server disconnected!\n");
            isRunning = false;
            break;
        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 非阻塞模式下没有数据可读，短暂休眠后继续
            this_thread::sleep_for(chrono::milliseconds(100));
        } else {
            // 其他错误
            perror("read error");
            isRunning = false;
            break;
        }
    }
}

int main() {
    // 创建Socket对象，并连接到服务器
    Socket *sock = new Socket();
    ItnetAddress *addr = new ItnetAddress("127.0.0.1", 1234);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    // 启动接收线程
    thread receiverThread(receiveMessages, sockfd, readBuffer);

    while(isRunning){
        sendBuffer->getline();
        // 发送数据
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        // quit
        if (strcmp(sendBuffer->c_str(), "quit") == 0) {
            printf("Client exited.\n");
            isRunning = false;
            break;
        }
    }
    // 等待接收线程退出
    if (receiverThread.joinable()) {
        receiverThread.join();
    }
    
    delete addr;
    delete sock;
    delete sendBuffer;
    delete readBuffer;

    return 0;
}