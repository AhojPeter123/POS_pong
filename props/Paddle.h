// paddle.h
#ifndef PADDLE_H
#define PADDLE_H

#include "../game/Position.h"

class Paddle {
private:
    Position position;
    int width, height;

public:
    Paddle(int startX, int startY, int w, int h);
    void moveUp();
    void moveDown();
    void draw();
    Position getPosition();
};

#endif // PADDLE_H
