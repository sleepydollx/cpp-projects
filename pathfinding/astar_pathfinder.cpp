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
    }