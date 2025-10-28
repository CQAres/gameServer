#pragma once
#include "lib/Connection.h"
#include <string>

class Game {
public:
    Game(Connection* player1, Connection* player2);
    void handleMove(Connection* player, std::string& move);

private:
    Connection* player1;
    Connection* player2;
    Connection* currentPlayer; // 当前回合的玩家
    bool isGameOver;
};