#include "Game.h"

Game::Game(std::weak_ptr<Connection> player1, std::weak_ptr<Connection> player2)
: player1(player1), player2(player2), currentPlayer(player1) {
    isGameOver = false;
}

void Game::handleMove(std::weak_ptr<Connection> _player, std::string& move) {
    auto player = _player.lock();
    auto current = currentPlayer.lock();
    if (!player || !current) return;

    // 不是当前玩家的回合
    if (player != current) {
        std::weak_ptr<Connection> wp = player;
        if (auto loop = player->getLoop()) {
            loop->queueInLoop([wp]() {
                if (auto sp = wp.lock()) {
                    sp->send(sp->getSockfd(), "Not your turn!\n");
                }
            });
        }
        return;
    }

    // 处理玩家操作
    printf("Player %d made move: %s\n", player->getSockfd(), move.c_str());

    auto p1 = this->player1.lock();
    auto p2 = this->player2.lock();
    if (!p1 || !p2) {
        isGameOver = true;
        return;
    }

    // 切换回合并通知对手
    auto nextPlayer = current == p1? p2 : p1;
    if (auto loop = nextPlayer->getLoop()) {
        std::weak_ptr<Connection> wp = nextPlayer;
        loop->queueInLoop([wp]() {
            if (auto sp = wp.lock()) {
                sp->send(sp->getSockfd(), "Your turn!\n");
            }
        });
    }
    
    currentPlayer = nextPlayer;
}