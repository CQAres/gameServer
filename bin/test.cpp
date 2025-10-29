#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>
#include "util/util.h"
#include "util/Buffer.h"
#include "util/ThreadPool.h"
#include "network/Socket.h"

using namespace std;

void oneClient(int msgs, int wait){
    Socket *sock = new Socket();
    ItnetAddress *addr = new ItnetAddress("127.0.0.1", 1234);
    // 客户端使用阻塞式连接比较好，方便简单不容易出错
    sock->connect(addr);

    int sockfd = sock->getFd();

    Buffer *sendBuffer = new Buffer();
    Buffer *readBuffer = new Buffer();

    sleep(wait);
    int count = 0;
    while(count < msgs){
        sendBuffer->setBuf("I'm client!");
        ssize_t write_bytes = write(sockfd, sendBuffer->c_str(), sendBuffer->size());
        if(write_bytes == -1){
            printf("socket already disconnected, can't write any more!\n");
            break;
        }
        int already_read = 0;
        std::vector<char> buf(128);
        while(true){
            bzero(buf.data(), buf.size());
            ssize_t read_bytes = read(sockfd, buf.data(), buf.size());
            if(read_bytes > 0){
                readBuffer->append(buf.data(), read_bytes);
                already_read += read_bytes;
            } else if(read_bytes == 0){
                printf("server disconnected!\n");
                exit(EXIT_SUCCESS);
            }
            if(already_read >= sendBuffer->size()){
                printf("count: %d, message from server: %s\n", count++, readBuffer->c_str());
                break;
            } 
        }
        readBuffer->clear();
    }
    delete addr;
    delete sock;
}

int main(int argc, char *argv[]) {
    int threads = 10000;
    int msgs = 10000;
    int wait = 0;
    int o;
    const char *optstring = "t:m:w:";
    while ((o = getopt(argc, argv, optstring)) != -1) {
        switch (o) {
            case 't':
                threads = stoi(optarg);
                break;
            case 'm':
                msgs = stoi(optarg);
                break;
            case 'w':
                wait = stoi(optarg);
                break;
            case '?':
                printf("error optopt: %c\n", optopt);
                printf("error opterr: %d\n", opterr);
                break;
        }
    }

    ThreadPool *poll = new ThreadPool(threads);
    std::function<void()> func = std::bind(oneClient, msgs, wait);
    for(int i = 0; i < threads; ++i){
        poll->add(func);
    }
    delete poll;
    return 0;
}