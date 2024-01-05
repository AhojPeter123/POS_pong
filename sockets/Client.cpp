#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ncurses.h>

class Client {
private:
    int clientSocket;

public:
    Client() {
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            std::cerr << "Error creating socket\n";
        }
    }

    void connectToServer(const char* serverIp, int port) {
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, serverIp, &serverAddr.sin_addr);

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Error connecting to server\n";
        }
    }

    void sendToServer(const char* message) {
        send(clientSocket, message, strlen(message), 0);
    }

    std::string receiveFromServer() {
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

    void closeClient() {
        close(clientSocket);
    }

    void handleUserInput(int& clientPaddleY) {
        int ch = getch();

        switch (ch) {
            case 'w':
                clientPaddleY -= 1;
                break;
            case 's':
                clientPaddleY += 1;
                break;
            default:
                break;
        }
    }

    void gameLoop() {
        int clientPaddleY = 10;
        int serverPaddleY = 10;
        int ballX = 50;
        int ballY = 25;

        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();

        timeout(0); // Set non-blocking input

        while (true) {
            handleUserInput(clientPaddleY);

            sendToServer(std::to_string(clientPaddleY).c_str());

            std::string gameState = receiveFromServer();

            sscanf(gameState.c_str(), "%d,%d,%d", &ballX, &ballY, &serverPaddleY);

            // Output positions for demonstration
            std::cout << "Ball Position: X=" << ballX << ", Y=" << ballY << std::endl;
            std::cout << "Client Paddle Position: Y=" << clientPaddleY << std::endl;
            std::cout << "Server Paddle Position: Y=" << serverPaddleY << std::endl;

            clientPaddleY++;
        }

        closeClient();
        endwin();
    }
};

int main() {
    Client client;
    client.connectToServer("127.0.0.1", 8080); // Connect to server at 127.0.0.1:8080

    client.gameLoop(); // Start the game loop

    return 0;
}
