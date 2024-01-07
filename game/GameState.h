#ifndef GAMESTATE_H
#define GAMESTATE_H

struct GameState {
    int ballX;
    int ballY;
    int paddle1;
    int paddle2;
    bool running;
    int score1;
    int score2;
};

#endif // GAMESTATE_H
