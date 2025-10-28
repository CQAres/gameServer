#include "lib/EventLoop.h"
#include "lib/Server.h"
#include <thread>

void monitorPerformance(Server* server) {
    while (true) {
        int connectionCount = server->getConnectionCount();
        printf("[Server Monitor] Current aliveConnection size: %d\n", connectionCount);
        server->monitorMatchPool();

        // 间隔 5 秒
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);

    // 启动性能监控线程
    std::thread monitorThread(monitorPerformance, server);
    monitorThread.detach();

    loop->loop();

    server->~Server();
    loop->~EventLoop();
    return 0;
}