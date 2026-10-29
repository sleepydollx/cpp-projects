#include <iostream>
#include <winsock.h>
#include <ws2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData;
    SOCKET serverSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;

    sockaddr_in serverService;
    sockaddr_in clientService;
    int clientAddrLen = sizeof(clientService);

    char buffer[1024] = {0};

    cout << " === LIGHTWEIGHT LOCAL CONSOLE SERVER ===" << endl;

    // 1. Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsasData) != 0) {
        cout << " [ERROR] Failed to initialize Winsock." << endl;
        return 1;
    }
    // 2. Create a socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << " [ERROR] Failed to create socket." << endl;
        WSACleanup();
        return 1;
    }

    // 3. Configure the server address structure
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverService.sin_port = htons(8080);

    // 4. Bind the socket
    if (bind(serverSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
        cout << " [ERROR] Failed to bind socket." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 5. Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << " [ERROR] Listen Failed." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    cout << "[STATUS] Server listening on port 8080..." << endl;
    cout << "[STATUS] Open browser or test with curl to send data." << endl;

    // 6. Accept a client connection
    clientSocket = accept(serverSocket, (SOCKADDR*)&clientService, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        cout << " [ERROR] Failed to accept client connection." << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "\n[!] Client connected!" << endl;

    // 7. Receive data from the client
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        cout << "[RECEIVED] " << buffer << endl;

    }

    // 8. Send HTTP response back to client
    string httpResponse = 
    "HTTP/1.1 200 OK\r\n
    Content-Type: text/plain\r\n
    Connection: close\r\n\r\n
    Hello, from your custom c++ server!";

    send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    cout << "\n[STATUS] Response sent to client." << endl;

    // 9. Clean up resources
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    cout << "[STATUS] Server shut down gracefully." << endl;
    return 0;
}

