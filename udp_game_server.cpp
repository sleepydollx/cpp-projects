#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

// Grid boundary constants
const int MAP_MIN = 0;
const int MAP_MAX = 50;

// Structure to track full player state and network endpoint
struct PlayerEndpoint {
    int id;
    string name;
    sockaddr_in address;
    int x = 10;
    int y = 10;
    int hp = 100;
    bool isAlive = true;
};

// Helper function to compare two client network addresses
bool areAddressesEqual(const sockaddr_in& a, const sockaddr_in& b) {
    return (a.sin_addr.s_addr == b.sin_addr.s_addr) && (a.sin_port == b.sin_port);
}

int main() {
    WSADATA wsaData;
    SOCKET udpSocket = INVALID_SOCKET;
    sockaddr_in serverService;

    cout << "=== ADVANCED UDP MULTIPLAYER GAME SERVER ===" << endl;

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

    cout << "[STATUS] Advanced UDP Server running on port 9600..." << endl;
    cout << "[INFO] Commands: JOIN <name>, MOVE <dx> <dy>, SHOOT <targetId> <dmg>, HEAL, RESPAWN, LEAVE\n" << endl;

    vector<PlayerEndpoint> players;
    char buffer[1024];
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int nextPlayerId = 1;

    // 5. Main UDP Game Loop
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&clientAddr, &clientAddrLen);
        if (bytesReceived <= 0) continue;

        buffer[bytesReceived] = '\0';
        string packetStr(buffer);

        // Find existing client index by IP and Port
        int currentPlayerIndex = -1;
        for (size_t i = 0; i < players.size(); ++i) {
            if (areAddressesEqual(players[i].address, clientAddr)) {
                currentPlayerIndex = static_cast<int>(i);
                break;
            }
        }

        stringstream ss(packetStr);
        string command;
        ss >> command;

        // Handle New Connection (Must call JOIN first)
        if (currentPlayerIndex == -1) {
            if (command == "JOIN") {
                string playerName;
                ss >> playerName;
                if (playerName.empty()) playerName = "Player_" + to_string(nextPlayerId);

                PlayerEndpoint newPlayer;
                newPlayer.id = nextPlayerId++;
                newPlayer.name = playerName;
                newPlayer.address = clientAddr;

                players.push_back(newPlayer);
                currentPlayerIndex = static_cast<int>(players.size()) - 1;

                cout << "[JOIN] " << newPlayer.name << " (ID: " << newPlayer.id << ") entered the arena!" << endl;
            } else {
                string err = "ERROR: You must JOIN first! Usage: JOIN <your_name>\n";
                sendto(udpSocket, err.c_str(), err.length(), 0, (const SOCKADDR*)&clientAddr, sizeof(clientAddr));
                continue;
            }
        }

        PlayerEndpoint& sender = players[currentPlayerIndex];

        // Process Game Actions
        if (command == "MOVE" && sender.isAlive) {
            int dx = 0, dy = 0;
            ss >> dx >> dy;

            // Clamping position within map limits (0 to 50)
            sender.x = max(MAP_MIN, min(MAP_MAX, sender.x + dx));
            sender.y = max(MAP_MIN, min(MAP_MAX, sender.y + dy));

            cout << "[MOVE] " << sender.name << " -> Pos: (" << sender.x << ", " << sender.y << ")" << endl;
        }
        else if (command == "SHOOT" && sender.isAlive) {
            int targetId = 0, damage = 0;
            ss >> targetId >> damage;

            for (auto& target : players) {
                if (target.id == targetId && target.isAlive && target.id != sender.id) {
                    target.hp -= damage;
                    cout << "[COMBAT] " << sender.name << " shot " << target.name << " for " << damage << " damage!" << endl;

                    if (target.hp <= 0) {
                        target.hp = 0;
                        target.isAlive = false;
                        cout << "[DEATH] " << target.name << " was defeated by " << sender.name << "!" << endl;
                    }
                    break;
                }
            }
        }
        else if (command == "HEAL" && sender.isAlive) {
            sender.hp = min(100, sender.hp + 25);
            cout << "[HEAL] " << sender.name << " recovered HP to " << sender.hp << endl;
        }
        else if (command == "RESPAWN" && !sender.isAlive) {
            sender.hp = 100;
            sender.isAlive = true;
            sender.x = 10;
            sender.y = 10;
            cout << "[RESPAWN] " << sender.name << " has respawned into the arena!" << endl;
        }
        else if (command == "LEAVE") {
            cout << "[LEAVE] " << sender.name << " disconnected." << endl;
            players.erase(players.begin() + currentPlayerIndex);
        }

        // Construct Broadcast Snapshot Payload
        stringstream stateSS;
        stateSS << "=== ARENA SNAPSHOT ===\n";
        for (const auto& p : players) {
            stateSS << "ID:" << p.id << " | " << p.name 
                    << " | Pos:(" << p.x << "," << p.y << ")"
                    << " | HP:" << p.hp 
                    << " | Status:" << (p.isAlive ? "ALIVE" : "DEAD") << "\n";
        }
        stateSS << "======================\n";

        string stateStr = stateSS.str();

        // Broadcast to all active players
        for (const auto& p : players) {
            sendto(udpSocket, stateStr.c_str(), stateStr.length(), 0, (const SOCKADDR*)&p.address, sizeof(p.address));
        }
    }

    closesocket(udpSocket);
    WSACleanup();
    return 0;
}