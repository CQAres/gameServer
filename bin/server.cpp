#include "lib/EventLoop.h"
#include "lib/Server.h"

int main() {
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    loop->loop();

    server->~Server();
    loop->~EventLoop();
    return 0;
}