// game.h
#ifndef GAME_H
#define GAME_H

#include "../props/Ball.h"
#include "../props/Paddle.h"

class Game {
private:
    bool isRunning;
    Ball ball;
    Paddle player1;
    Paddle player2;

public:
    Game();
    void init();
    void handleInput();
    void update();
    void render();
    void run();
};

#endif // GAME_H
