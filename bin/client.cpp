#include "util/util.h"
#include "util/Buffer.h"
#include "network/Socket.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

int main() {
    // 创建Socket对象，并连接到服务器
    Socket *sock = new Socket();
    ItnetAddress *addr = new ItnetAddress("127.0.0.1", 1234);
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();
    
    while(true){
        sendBuffer->getline();
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        std::vector<char> buf(1024);
        while(true){
            bzero(buf.data(), buf.size());
            ssize_t read_bytes = read(sockfd, buf.data(), buf.size());
            if(read_bytes > 0){
                readBuffer->append(buf.data(), read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){         //EOF
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()){
                printf("message from server: %s\n", readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
    }
    delete addr;
    delete sock;
    return 0;
}