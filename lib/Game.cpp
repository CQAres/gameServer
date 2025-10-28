#include "Game.h"
#include "Connection.h"

Game::Game(Connection* player1, Connection* player2)
: player1(player1), player2(player2), currentPlayer(player1) {
    isGameOver = false;
}

void Game::handleMove(Connection* player, std::string& move) {
    int sockfd = player->getSockfd();
    if (player != currentPlayer) {
        // 不是当前玩家的回合
        player->send(sockfd, "Not your turn!\n");
        return;
    }
    // 处理玩家操作
    printf("Player %d made move: %s\n", sockfd, move.c_str());

    // 切换回合并通知对手
    Connection* nextPlayer = currentPlayer == player1? player2 : player1;
    int opponentSock = nextPlayer->getSockfd();
    nextPlayer->send(opponentSock, "Opponent made move: " + move + "\n");
}