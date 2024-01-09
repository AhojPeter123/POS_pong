//
// Created by kubov on 8. 1. 2024.
//

#include <SDL.h>
#include <cmath>

class Ball {

public:
Ball(int initialX, int initialY, int radius, int velX, int velY) : x(initialX), y(initialY), radius(radius), velX(velX), velY(velY) {}


void draw(SDL_Renderer* renderer, int x, int y) {
    // Draw the ball as a filled circle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
    for (int i = 0; i < 360; ++i) {
        double angle = i * M_PI / 180.0;
        int drawX = static_cast<int>(x + radius * std::cos(angle));
        int drawY = static_cast<int>(y + radius * std::sin(angle));
        SDL_RenderDrawPoint(renderer, drawX, drawY);
    }
}



private:
int x;
int y;
int radius;
int velX;
int velY;

};
