cmake_minimum_required(VERSION 3.18)
project(semestralka_pong)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        semestralka_pong
        game/main.cpp
        props/Ball.h
        props/Ball.cpp
        game/Game.h
        game/Game.cpp
        props/Paddle.h
        props/Paddle.cpp
        game/Position.h
        game/Position.cpp
        game/Player.h
        game/Player.cpp
        sockets/Client.cpp
        sockets/Server.cpp
)

