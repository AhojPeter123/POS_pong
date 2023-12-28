// ball.h
#ifndef BALL_H
#define BALL_H

#include "../game/Position.h"

class Ball {
private:
    Position position;
    int speedX, speedY;

public:
    Ball(int startX, int startY, int spdX, int spdY);
    void move();
    void draw();
    Position getPosition();
};

#endif // BALL_H
