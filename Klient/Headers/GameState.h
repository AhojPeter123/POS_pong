//
// Created by kubov on 8. 1. 2024.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

struct GameState {
    int ballX;
    int ballY;
    int paddle1y;
    int paddle2y;
    int paddle1x;
    int paddle2x;
    bool running;
    int score1;
    int score2;
    int ballSpeedX;
    int ballSpeedY;
    int paddleHeight;
    int paddleWidth;
};

#endif // GAMESTATE_H
