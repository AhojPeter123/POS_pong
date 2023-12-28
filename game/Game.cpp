
#include "Game.h"

Game::Game() {

}

void Game::init() {

}

void Game::handleInput() {

}

void Game::update() {

}


void Game::render() {

}

void Game::run() {
    init();
    while (isRunning) {
        handleInput();
        update();
        render();
    }
}
