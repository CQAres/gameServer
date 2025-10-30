#pragma once
#include "Connection.h"
#include "EventLoop.h"
#include <string>
#include <memory>

class Game {
private:
    std::weak_ptr<Connection> player1;
    std::weak_ptr<Connection> player2;
    std::weak_ptr<Connection> currentPlayer; // 当前回合的玩家
    bool isGameOver;
    
public:
    Game(std::weak_ptr<Connection> player1, std::weak_ptr<Connection> player2);
    void handleMove(std::weak_ptr<Connection> _player, std::string& move);
};