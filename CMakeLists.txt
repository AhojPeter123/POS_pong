cmake_minimum_required(VERSION 3.18)
project(semestralka_pong)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        semestralka_pong
        sockets/Server.cpp
)

target_link_libraries(semestralka_pong ncurses)