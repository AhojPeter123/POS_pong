#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ncurses.h>

class Server {
private:
    int serverSocket;
    int clientSocket;

public:
    Server() {
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

        if (listen(serverSocket, 1) == -1) {
            std::cerr << "Error listening\n";
        }

        std::cout << "Waiting for client to connect...\n";

        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection\n";
        }

        std::cout << "Client connected\n";
    }

    void sendToClient(const char* message) {
        send(clientSocket, message, strlen(message), 0);
    }

    std::string receiveFromClient() {
        char buffer[1024];
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == -1) {
            std::cerr << "Error receiving data\n";
            return "";
        } else {
            buffer[bytesReceived] = '\0';
            return std::string(buffer);
        }
    }

    void closeServer() {
        close(clientSocket);
        close(serverSocket);
    }

    void handleUserInput(int& serverPaddleY) {
        int ch = getch();

        switch (ch) {
            case KEY_UP:
                serverPaddleY -= 1;
                break;
            case KEY_DOWN:
                serverPaddleY += 1;
                break;
            default:
                break;
        }
    }

    void gameLoop() {
        int serverPaddleY = 10;
        int clientPaddleY = 10;
        int ballX = 50;
        int ballY = 25;

        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();

        timeout(0); // Set non-blocking input

        while (true) {
            handleUserInput(serverPaddleY);

            ballX = 50; // For demo, assuming the ball position remains constant
            ballY = 25; // For demo, assuming the ball position remains constant

            sendGameState(ballX, ballY, serverPaddleY, clientPaddleY);

            std::string receivedClientPaddleY = receiveFromClient();
            if (!receivedClientPaddleY.empty()) {
                sscanf(receivedClientPaddleY.c_str(), "%d", &clientPaddleY);
            }

            // Output positions for demonstration
            std::cout << "Ball Position: X=" << ballX << ", Y=" << ballY << std::endl;
            std::cout << "Server Paddle Position: Y=" << serverPaddleY << std::endl;

            // Update client paddle position
            std::cout << "Client Paddle Position (Updated by Server): Y=" << clientPaddleY << std::endl;
            serverPaddleY++;
        }

        closeServer();
        endwin();
    }


    void sendGameState(int ballX, int ballY, int serverPaddleY, int clientPaddleY) {
        std::string gameState = std::to_string(ballX) + "," + std::to_string(ballY) + "," + std::to_string(serverPaddleY) + "," + std::to_string(clientPaddleY);
        sendToClient(gameState.c_str());
    }
};

int main() {
    Server server;
    server.startServer(8080); // Start server on port 8080

    server.gameLoop(); // Start the game loop

    return 0;
}
