#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

const int MAP_WIDTH = 50;
const int MAP_HEIGHT = 22;
const int MAX_ROOMS = 8;
const int MIN_ROOM_SIZE = 4;
const int MAX_ROOM_SIZE = 8;

struct Room {
    int x, y, width, height;

    int centerX() const { return x + width / 2; }
    int centerY() const { return y + height / 2; }

    // Check overlap with 1-tile wall padding between rooms
    bool intersects(const Room& other) const {
        return (x - 1 <= other.x + other.width &&
                x + width + 1 >= other.x &&
                y - 1 <= other.y + other.height &&
                y + height + 1 >= other.y);
    }
};

void carveRoom(vector<vector<char>>& grid, const Room& room) {
    for (int y = room.y; y < room.y + room.height; ++y) {
        for (int x = room.x; x < room.x + room.width; ++x) {
            grid[y][x] = '.';
        }
    }
}

void carveHorizontalTunnel(vector<vector<char>>& grid, int x1, int x2, int y) {
    for (int x = min(x1, x2); x <= max(x1, x2); ++x) {
        grid[y][x] = '.';
    }
}

void carveVerticalTunnel(vector<vector<char>>& grid, int y1, int y2, int x) {
    for (int y = min(y1, y2); y <= max(y1, y2); ++y) {
        grid[y][x] = '.';
    }
}

int main() {
    // Initialize random number generator (Mersenne Twister)
    random_device rd;
    mt19937 gen(rd());

    // Fill entire map with solid walls '#'
    vector<vector<char>> dungeon(MAP_HEIGHT, vector<char>(MAP_WIDTH, '#'));
    vector<Room> rooms;

    uniform_int_distribution<> sizeDist(MIN_ROOM_SIZE, MAX_ROOM_SIZE);

    for (int i = 0; i < MAX_ROOMS * 3; ++i) {
        if (static_cast<int>(rooms.size()) >= MAX_ROOMS) break;

        int w = sizeDist(gen);
        int h = sizeDist(gen);

        uniform_int_distribution<> xDist(1, MAP_WIDTH - w - 2);
        uniform_int_distribution<> yDist(1, MAP_HEIGHT - h - 2);

        Room newRoom = {xDist(gen), yDist(gen), w, h};

        bool overlaps = false;
        for (const auto& other : rooms) {
            if (newRoom.intersects(other)) {
                overlaps = true;
                break;
            }
        }

        if (!overlaps) {
            carveRoom(dungeon, newRoom);

            // Connect new room to the previous room using L-shaped corridors
            if (!rooms.empty()) {
                int prevX = rooms.back().centerX();
                int prevY = rooms.back().centerY();
                int currX = newRoom.centerX();
                int currY = newRoom.centerY();

                if (gen() % 2 == 0) {
                    carveHorizontalTunnel(dungeon, prevX, currX, prevY);
                    carveVerticalTunnel(dungeon, prevY, currY, currX);
                } else {
                    carveVerticalTunnel(dungeon, prevY, currY, prevX);
                    carveHorizontalTunnel(dungeon, prevX, currX, currY);
                }
            }

            rooms.push_back(newRoom);
        }
    }

    // Populate entities inside generated rooms
    if (!rooms.empty()) {
        // Player spawn in the first room
        dungeon[rooms.front().centerY()][rooms.front().centerX()] = '@';

        // Treasure chest in the final room
        if (rooms.size() > 1) {
            dungeon[rooms.back().centerY()][rooms.back().centerX()] = '$';
        }

        // Enemies in the middle rooms
        for (size_t i = 1; i + 1 < rooms.size(); ++i) {
            dungeon[rooms[i].centerY()][rooms[i].centerX()] = 'E';
        }
    }

    // Render dungeon to terminal
    cout << "=== PROCEDURAL ROGUELIKE DUNGEON GENERATOR ===" << endl;
    cout << "Legend: [@] Player | [E] Enemy | [$] Loot | [.] Floor | [#] Wall\n" << endl;

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            cout << dungeon[y][x];
        }
        cout << endl;
    }

    cout << "\n[INFO] Generated " << rooms.size() << " connected rooms." << endl;
    return 0;
}