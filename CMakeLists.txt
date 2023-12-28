cmake_minimum_required(VERSION 3.18)
project(semestralka_pong)

set(CMAKE_CXX_STANDARD 17)

add_executable(semestralka_pong game/main.cpp
        props/Ball.h
        game/Game.h
        props/Paddle.h
        game/Position.h)

