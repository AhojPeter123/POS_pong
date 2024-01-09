#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include "GameState.h"

const int MAX_X = 800;
const int MAX_Y = 500;


class Server {
private:
    int serverSocket;
    int clientSocket1;
    int clientSocket2;
    std::mutex stateMutex;
    std::mutex paddle1Lock;
    std::mutex paddle2Lock;
    std::condition_variable cond;
    GameState gameState;

    void setClientPaddlePosition1(std::mutex& paddleLock, std::condition_variable& cond) {

        int receivedPaddle;
        while (gameState.running) {

            recv(clientSocket1, &receivedPaddle, sizeof(int), 0);

            if(receivedPaddle == -4) {
                closeServer();
            }

           {
                std::lock_guard<std::mutex> lock(paddleLock);
                gameState.paddle1y = receivedPaddle;
            }

            cond.notify_all();
            usleep(1000);

        }
    }void setClientPaddlePosition2(std::mutex& paddleLock, std::condition_variable& cond) {

        int receivedPaddle;
        while (gameState.running) {

            recv(clientSocket2, &receivedPaddle, sizeof(int), 0);

            {
                std::lock_guard<std::mutex> lock(paddleLock);
                gameState.paddle2y = receivedPaddle;
            }

            // Notify the other thread that the message has been sent
            cond.notify_all();
            usleep(1000);

        }
    }

    void moveBall() {

        {
            std::lock_guard<std::mutex> lock(stateMutex);
            gameState.ballX += gameState.ballSpeedX;
            gameState.ballY += gameState.ballSpeedY;
        }
            // Wall collision detection
            if (gameState.ballY <= 0 || gameState.ballY >= MAX_Y) {
                {
                    std::lock_guard<std::mutex> lock(stateMutex);
                    gameState.ballSpeedY = -gameState.ballSpeedY;
                }
                cond.notify_all();
            }
            // Scoring
            if (gameState.ballX <= 0) {
                {
                    std::lock_guard<std::mutex> lock(stateMutex);
                    gameState.score2++;
                    if(gameState.score2 == 5) {
                        closeServer();
                    }
                }
                resetBallPosition();
            } else if (gameState.ballX >= MAX_X) {
                {
                    std::lock_guard<std::mutex> lock(stateMutex);
                    gameState.score1++;

                    if(gameState.score2 == 5) {
                        closeServer();
                    }

                }
                resetBallPosition();
            }


        checkPaddleCollisions();
    }

    void resetBallPosition() {
        std::cout << "Player1: " << gameState.score1 << " Player2:" << gameState.score2 << std::endl;
        {
            std::lock_guard<std::mutex> lock(stateMutex);

            gameState.ballX = MAX_X / 2;
            gameState.ballY = MAX_Y / 2;
            gameState.ballSpeedX = 1;
            gameState.ballSpeedY = 1;
        }
        cond.notify_all();
    }

    void checkPaddleCollisions() {
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            // Check collision with player 1's paddle
            if (gameState.ballX == gameState.paddle1x + gameState.paddleWidth &&
                gameState.ballY >= gameState.paddle1y &&
                gameState.ballY <= gameState.paddle1y + gameState.paddleHeight) {

                gameState.ballSpeedX = -gameState.ballSpeedX;
            }

            // Check collision with player 2's paddle
            if (gameState.ballX == gameState.paddle2x &&
                gameState.ballY >= gameState.paddle2y &&
                gameState.ballY <= gameState.paddle2y + gameState.paddleHeight) {

                gameState.ballSpeedX = -gameState.ballSpeedX;
            }
        }
        cond.notify_all();


    }

public:
    Server() {
        gameState.ballX = 400;
        gameState.ballY = 200;
        gameState.paddle1y = 100;
        gameState.paddle2y = 200;
        gameState.paddle2x = 730;
        gameState.paddle1x = 50;
        gameState.ballSpeedX = 1;
        gameState.ballSpeedY = 1;
        gameState.score1 = 0;
        gameState.score2 = 0;
        gameState.running = true;
        gameState.paddleWidth = 10;
        gameState.paddleHeight = 120;

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == -1) {
            std::cerr << "Error creating socket\n";
        }
    }

    void startServer(int port) {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Error binding\n";
        }

        if (listen(serverSocket, 2) == -1) {
            std::cerr << "Error listening\n";
        }

        std::cout << "Waiting for clients to connect...\n";

        sockaddr_in clientAddr1, clientAddr2;
        socklen_t clientAddrLen;

        clientSocket1 = accept(serverSocket, (sockaddr*)&clientAddr1, &clientAddrLen);
        if (clientSocket1 == -1) {
            std::cerr << "Error accepting first client connection\n";
        }
        std::cout << "First client connected\n";

        clientSocket2 = accept(serverSocket, (sockaddr*)&clientAddr2, &clientAddrLen);
        if (clientSocket2 == -1) {
            std::cerr << "Error accepting second client connection\n";
        }
        std::cout << "Second client connected\n";

        std::thread thread1(&Server::setClientPaddlePosition1, this, std::ref(paddle1Lock), std::ref(cond));

        std::thread thread2(&Server::setClientPaddlePosition2, this, std::ref(paddle2Lock), std::ref(cond));

        thread1.detach();
        thread2.detach();

        std::thread gameLoopThread(&Server::gameLoop, this);
        gameLoopThread.detach();

        gameLoop();

    }

    void sendGameState() {
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            send(clientSocket1, &gameState, sizeof(GameState), 0);
            send(clientSocket2, &gameState, sizeof(GameState), 0);

        }
        cond.notify_all();
    }

    void closeServer() {
        gameState.running = false;
        close(clientSocket1);
        close(clientSocket2);
        close(serverSocket);
    }

    void gameLoop() {
        while (gameState.running) {
            moveBall();
            sendGameState();
            usleep(10000);
        }
    }
};

int main() {
    Server server;
    server.startServer(18888);

    server.gameLoop();

    return 0;
}