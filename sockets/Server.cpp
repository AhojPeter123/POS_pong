#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include "../game/GameState.h"

class Server {
private:
    int serverSocket;
    int clientSocket1;
    int clientSocket2;
    std::mutex paddleLock1;
    std::mutex paddleLock2;
    GameState gameState;

    void setClientPaddlePosition(int clientSocket, int& paddle, std::mutex& paddleLock) {
        while (gameState.running) {
            int receivedInt;
            ssize_t bytesReceived = recv(clientSocket, &receivedInt, sizeof(receivedInt), 0);
            if (bytesReceived == -1) {
                std::cerr << "Error receiving data\n";
            } else {
                std::lock_guard<std::mutex> lock(paddleLock);
                std::cout << "Received: " << receivedInt << " from client " << clientSocket << std::endl;

                paddle = receivedInt;
            }
        }
    }

public:
    Server() {
        gameState.ballX = 50;
        gameState.ballY = 25;
        gameState.paddle1 = 10;
        gameState.paddle2 = 10;
        gameState.running = 1;
        gameState.score1 = 0;
        gameState.score2 = 0;

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

        if (listen(serverSocket, 2) == -1) { // Listen for two clients
            std::cerr << "Error listening\n";
        }

        std::cout << "Waiting for clients to connect...\n";

        sockaddr_in clientAddr1, clientAddr2;
        socklen_t clientAddrLen = sizeof(clientAddr1);

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

        std::thread thread1(&Server::setClientPaddlePosition,
                            this, clientSocket1,
                            std::ref(gameState.paddle1),
                            std::ref(paddleLock1));

        std::thread thread2(&Server::setClientPaddlePosition,
                            this, clientSocket2,
                            std::ref(gameState.paddle2),
                            std::ref(paddleLock2));

        thread1.detach();
        thread2.detach();
    }

    void sendGameState(int clientSocket, const GameState& gameState) {
        send(clientSocket, &gameState, sizeof(gameState), 0);
    }

    void closeServer() {
        close(clientSocket1);
        close(clientSocket2);
        close(serverSocket);
    }

    void gameLoop() {
        while (gameState.running) {

            sendGameState(clientSocket1, gameState);
            sendGameState(clientSocket2, gameState);

            std::cout << "Player 1 Paddle Position: " << gameState.paddle1 << std::endl;
            std::cout << "Player 2 Paddle Position: " << gameState.paddle2 << std::endl;

            usleep(10000);
        }

        closeServer();
    }
};

int main() {
    Server server;
    server.startServer(8080); // Start server on port 8080

    server.gameLoop(); // Start the game loop

    return 0;
}
