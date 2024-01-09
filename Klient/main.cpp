#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>
#include <SDL.h>
#include "Headers/Ball.h"
#include "Headers/GameState.h"


#pragma comment(lib, "ws2_32.lib")


class Client {
private:
    SOCKET clientSocket;
    GameState gameState;
    std::mutex stateMutex;
    int paddle1Y = 100;

public:
    Client() {
        gameState.running = true;
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed\n";
        }

        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket\n";
        }

        std::cout << "client " << std::endl;
    }
    void startUserInputThread() {
         printf("INPUT THREAD STARTED...");

         std::thread inputThread(&::Client::handleInputKey, this);
         inputThread.detach();
     }


    void connectToServer(const char* url, int port) {
        addrinfo* result = nullptr;
        addrinfo hints;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int addrInfoResult = getaddrinfo(url, std::to_string(port).c_str(), &hints, &result);
        if (addrInfoResult != 0) {
            std::cerr << "Getaddrinfo failed: " << addrInfoResult << std::endl;
            WSACleanup();
            return;
        }

        for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
            clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (clientSocket == INVALID_SOCKET) {
                std::cerr << "Error creating socket\n";
                WSACleanup();
                return;
            }

            if (connect(clientSocket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == SOCKET_ERROR) {
                closesocket(clientSocket);
                clientSocket = INVALID_SOCKET;
                continue;
            }

            break;
        }

        freeaddrinfo(result);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Unable to connect to server\n";
            WSACleanup();
            return;
        }

        std::cout << "Connected to server\n";
    }

    void receiveGameState() {
        std::cout <<"RECEIVE THREAD STARTED..."<< std::endl;

        GameState tempGameState;
        ssize_t bytesReceived = recv(clientSocket, reinterpret_cast<char*> (&tempGameState), sizeof(GameState), 0);

        if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
            std::cerr << "Error receiving game state or connection closed\n";
        }

        {
            gameState = tempGameState;
        }

    }

    void displayGameState(SDL_Renderer* renderer) {
        {
            Ball b = Ball(gameState.ballX, gameState.ballY, 10, gameState.ballSpeedX, gameState.ballSpeedY);

            SDL_Rect paddle1 = {gameState.paddle1x, gameState.paddle1y, gameState.paddleWidth, gameState.paddleHeight}; // X, Y, Width, Height
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
            SDL_RenderDrawRect(renderer, &paddle1);

            SDL_Rect paddle2 = {gameState.paddle2x, gameState.paddle2y, gameState.paddleWidth, gameState.paddleHeight}; // X, Y, Width, Height
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Set color to white
            SDL_RenderDrawRect(renderer, &paddle2);

            b.draw(renderer, gameState.ballX, gameState.ballY);
        }

    }
    void handleInputKey() {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            std::cout << "key pressed" << std::endl;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        paddle1Y = -4;
                        return;
                    case SDLK_UP:
                        if (paddle1Y > 0)
                        {
                            paddle1Y -= 15;
                            sendGameStateData();
                        }


                        break;
                    case SDLK_DOWN:
                        if (paddle1Y + gameState.paddleHeight < 500)
                        {
                            paddle1Y += 15;
                            sendGameStateData();
                        }

                        break;

                    default:
                        break;
                }
            }
        }
    }

    void sendGameStateData() {
        send(clientSocket, reinterpret_cast<char*>(&paddle1Y), sizeof(int), 0);
    }

    void gameLoop() {
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        SDL_Init(SDL_INIT_EVERYTHING);

        SDL_CreateWindowAndRenderer(800, 500, 0, &window, &renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        while(gameState.running) {
            handleInputKey();

            receiveGameState();

            displayGameState(renderer);
            SDL_RenderPresent(renderer);


            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

    }
};

int main(int argc, char* argv[]) {
    Client client;
    client.connectToServer("frios2.fri.uniza.sk", 18888);

    client.gameLoop();

    WSACleanup();
    return 0;
}