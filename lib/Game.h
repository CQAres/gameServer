#pragma once
#include "Connection.h"
#include <string>

class Game {
private:
    Connection* player1;
    Connection* player2;
    Connection* currentPlayer; // 当前回合的玩家
    bool isGameOver;
    
public:
    Game(Connection* player1, Connection* player2);
    void handleMove(Connection* player, std::string& move);
};