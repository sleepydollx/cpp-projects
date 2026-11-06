#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

// Structure to track connected player endpoints
struct PlayerEndpoint {
    int id;
    sockaddr_in address;
    int x = 0;
    int y = 0;
};

// Helper function to compare two client network addresses
bool areAddressesEqual(const sockaddr_in& a, const sockaddr_in& b) {
    return (a.sin_addr.s_addr == b.sin_addr.s_addr) && (a.sin_port == b.sin_port);
}

int main() {
    WSADATA wsaData;
    SOCKET udpSocket = INVALID_SOCKET;
    sockaddr_in serverService;

    cout << "=== UDP FAST MULTIPLAYER GAME SERVER ===" << endl;

    // 1. Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "[ERROR] Failed to initialize Winsock." << endl;
        return 1;
    }

    // 2. Create UDP Socket (SOCK_DGRAM)
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET) {
        cout << "[ERROR] Failed to create UDP socket." << endl;
        WSACleanup();
        return 1;
    }

    // 3. Configure Server Address (Port 9600)
    serverService.sin_family = AF_INET;
    serverService.sin_addr.s_addr = INADDR_ANY;
    serverService.sin_port = htons(9600);

    // 4. Bind Socket
    if (bind(udpSocket, (SOCKADDR*)&serverService, sizeof(serverService)) == SOCKET_ERROR) {
        cout << "[ERROR] Bind failed." << endl;
        closesocket(udpSocket);
        WSACleanup();
        return 1;
    }

    cout << "[STATUS] UDP Game Server listening on port 9600..." << endl;
    cout << "[INFO] Waiting for UDP datagram packets...\n" << endl;

    vector<PlayerEndpoint> players;
    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int nextPlayerId = 1;

    // 5. Main UDP Receive and Broadcast Loop
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        // recvfrom() waits for any incoming UDP packet and captures client address
        int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (bytesReceived <= 0) continue;

        buffer[bytesReceived] = '\0';
        string packetStr(buffer);

        // Find if this client IP & Port is already registered
        int currentPlayerIndex = -1;
        for (size_t i = 0; i < players.size(); ++i) {
            if (areAddressesEqual(players[i].address, clientAddr)) {
                currentPlayerIndex = static_cast<int>(i);
                break;
            }
        }

        // Register new player if unknown address
        if (currentPlayerIndex == -1) {
            PlayerEndpoint newPlayer;
            newPlayer.id = nextPlayerId++;
            newPlayer.address = clientAddr;
            players.push_back(newPlayer);
            currentPlayerIndex = static_cast<int>(players.size()) - 1;

            cout << "[NEW PLAYER] Registered Player ID: " << players[currentPlayerIndex].id << endl;
        }

        // Process movement input (e.g., "MOVE 1 -1")
        stringstream ss(packetStr);
        string command;
        ss >> command;

        if (command == "MOVE") {
            int dx = 0, dy = 0;
            ss >> dx >> dy;
            players[currentPlayerIndex].x += dx;
            players[currentPlayerIndex].y += dy;
        }

        // Construct high-speed state snapshot string
        stringstream stateSS;
        stateSS << "SNAPSHOT: ";
        for (const auto& p : players) {
            stateSS << "P" << p.id << "[" << p.x << "," << p.y << "] ";
        }
        stateSS << "\n";

        string stateStr = stateSS.str();

        // Broadcast snapshot to ALL players via sendto()
        for (const auto& p : players) {
            sendto(udpSocket, stateStr.c_str(), stateStr.length(), 0, (const SOCKADDR*)&p.address, sizeof(p.address));
        }
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}