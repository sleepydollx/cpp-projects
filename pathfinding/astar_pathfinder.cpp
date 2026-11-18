#include <iostream>
#include <vector> 
#include <queue> 
#include <cmath> 
#include <climits>
#include <utility> 

using namespace std;

const int ROWS = 10;
const int COLS = 10;

// alias for priority queue element: {fCost, {x, y}}
typedef pair<int, pair<int, int>> PQNode;

// manhattan distance heuristic
int calculateHeuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void runAStar(int grid [ROWS][COLS], pair<int, int> start, pair<int, int> goal) {
    bool closedList[ROWS][COLS] = {false};
    int gCost[ROWS][COLS];
    pair<int, int> parent[ROWS][COLS];
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            gCost[i][j] = INT_MAX;
            parent[i][j] = {-1, -1}; 
        }
    }

    // min-heap priority queue for open list
    priority_queue<PQNode, vector<PQNode>, greater<PQNode>> openList;

    int startX = start.first, startY = start.second;
    gCost[startX][startY] = 0;
    int initialF = calculateHeuristic(startX, startY, goal.first, goal.second);
    openList.push({initialF, {startX, startY}});

    // 4-directional movement (up, down, left, right)
    int dx[4]= {-1, 1, 0, 0};
    int dy[4]= {0, 0, -1, 1};

    bool pathFound = false;

    while (!openList.empty()) {
        PQNode current = openList.top();
        openList.pop();

        int x = current.second.first;
        int y = current.second.second;

        if (closedList[x][y]) continue;
        closedList[x][y] = true;

        // check if goal reached
        if (x == goal.first && y == goal.second) {
            pathFound = true;
            break;
        }

        // explore neighbors
        for (int i = 0; i <4; ++i) {
            int nx = x + dx[i];
            int ny = y + dy[i];

            // Validate grid bounds
            if (nx < 0 || nx >= ROWS || ny < 0 || ny >= COLS) continue;
            if (grid[nx][ny] == 1 || closedList[nx][ny]) continue;

            int tentativeG = gCost[x][y] + 1; // cost to move to neighbor

            if (tentativeG <gCost[nx][ny]) {
                gCost[nx][ny] = tentativeG; 
                int hCost = calculateHeuristic(nx, ny, goal.first, goal.second);
                int fCost = tentativeG + hCost;

                parent[nx][ny] = {x, y};
                openList.push({fCost, {nx, ny}});
            }

            } 
        }

        // prepare visual grid for output 
        char visualGrid[ROWS][COLS];
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                visualGrid[i][j] = (grid[i][j] == 1) ? '#' : '.';
            }
        }

        if (pathFound) {
            
        }
    }