// server.cpp
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


void playerHandler(int clientSocket) {

}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize = sizeof(struct sockaddr_in);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    listen(serverSocket, 5);

    std::cout << "Server spustený. Čakanie na hráča...\n";

    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);

    std::cout << "Hráč pripojený!\n";

    std::thread playerThread(playerHandler, clientSocket);
    playerThread.join();

    close(clientSocket);
    close(serverSocket);

    return 0;
}
