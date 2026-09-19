/*
 * Problem Description:
 * Implement A* Search Algorithm.
 * A* is an informed search algorithm used for pathfinding and graph traversal.
 * It uses a heuristic function to estimate the cost from current node to the
 * destination.
 */

#include <cfloat>
#include <cmath>
#include <cstring>
#include <iostream>
#include <set>
#include <stack>
#include <vector>

using namespace std;

#define ROW 9
#define COL 10

typedef pair<int, int> Pair;
typedef pair<double, pair<int, int>> pPair;

struct cell {
  int parent_i, parent_j;
  double f, g, h;
};

bool isValid(int row, int col) {
  return (row >= 0) && (row < ROW) && (col >= 0) && (col < COL);
}

bool isUnBlocked(int grid[][COL], int row, int col) {
  if (grid[row][col] == 1)
    return (true);
  else
    return (false);
}

bool isDestination(int row, int col, Pair dest) {
  if (row == dest.first && col == dest.second)
    return (true);
  else
    return (false);
}

double calculateHValue(int row, int col, Pair dest) {
  return ((double)sqrt((row - dest.first) * (row - dest.first) +
                       (col - dest.second) * (col - dest.second)));
}

void tracePath(cell cellDetails[][COL], Pair dest) {
  int row = dest.first;
  int col = dest.second;

  stack<Pair> Path;

  while (!(cellDetails[row][col].parent_i == row &&
           cellDetails[row][col].parent_j == col)) {
    Path.push(make_pair(row, col));
    int temp_row = cellDetails[row][col].parent_i;
    int temp_col = cellDetails[row][col].parent_j;
    row = temp_row;
    col = temp_col;
  }

  Path.push(make_pair(row, col));
  while (!Path.empty()) {
    pair<int, int> p = Path.top();
    Path.pop();
    cout << "-> (" << p.first << "," << p.second << ") ";
  }
  cout << "\n";
}

void aStarSearch(int grid[][COL], Pair src, Pair dest) {
  if (isValid(src.first, src.second) == false ||
      isValid(dest.first, dest.second) == false) {
    cout << "Source or destination is invalid\n";
    return;
  }
  if (isUnBlocked(grid, src.first, src.second) == false ||
      isUnBlocked(grid, dest.first, dest.second) == false) {
    cout << "Source or the destination is blocked\n";
    return;
  }
  if (isDestination(src.first, src.second, dest) == true) {
    cout << "We are already at the destination\n";
    return;
  }

  bool closedList[ROW][COL];
  memset(closedList, false, sizeof(closedList));

  cell cellDetails[ROW][COL];
  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COL; j++) {
      cellDetails[i][j].f = FLT_MAX;
      cellDetails[i][j].g = FLT_MAX;
      cellDetails[i][j].h = FLT_MAX;
      cellDetails[i][j].parent_i = -1;
      cellDetails[i][j].parent_j = -1;
    }
  }

  int i = src.first, j = src.second;
  cellDetails[i][j].f = 0.0;
  cellDetails[i][j].g = 0.0;
  cellDetails[i][j].h = 0.0;
  cellDetails[i][j].parent_i = i;
  cellDetails[i][j].parent_j = j;

  set<pPair> openList;
  openList.insert(make_pair(0.0, make_pair(i, j)));

  bool foundDest = false;

  while (!openList.empty()) {
    pPair p = *openList.begin();
    openList.erase(openList.begin());

    i = p.second.first;
    j = p.second.second;
    closedList[i][j] = true;

    int dRow[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dCol[] = {0, 0, 1, -1, 1, -1, 1, -1};

    for (int k = 0; k < 8; k++) {
      int newI = i + dRow[k];
      int newJ = j + dCol[k];

      if (isValid(newI, newJ)) {
        if (isDestination(newI, newJ, dest)) {
          cellDetails[newI][newJ].parent_i = i;
          cellDetails[newI][newJ].parent_j = j;
          cout << "The destination cell is found\n";
          tracePath(cellDetails, dest);
          foundDest = true;
          return;
        } else if (closedList[newI][newJ] == false &&
                   isUnBlocked(grid, newI, newJ)) {
          double gNew = cellDetails[i][j].g + 1.0;
          double hNew = calculateHValue(newI, newJ, dest);
          double fNew = gNew + hNew;

          if (cellDetails[newI][newJ].f == FLT_MAX ||
              cellDetails[newI][newJ].f > fNew) {
            openList.insert(make_pair(fNew, make_pair(newI, newJ)));
            cellDetails[newI][newJ].f = fNew;
            cellDetails[newI][newJ].g = gNew;
            cellDetails[newI][newJ].h = hNew;
            cellDetails[newI][newJ].parent_i = i;
            cellDetails[newI][newJ].parent_j = j;
          }
        }
      }
    }
  }
  if (foundDest == false)
    cout << "Failed to find the Destination Cell\n";
}

int main() {
  int grid[ROW][COL] = {
      {1, 0, 1, 1, 1, 1, 0, 1, 1, 1}, {1, 1, 1, 0, 1, 1, 1, 0, 1, 1},
      {1, 1, 1, 0, 1, 1, 0, 1, 0, 1}, {0, 0, 1, 0, 1, 0, 0, 0, 0, 1},
      {1, 1, 1, 0, 1, 1, 1, 0, 1, 0}, {1, 0, 1, 1, 1, 1, 0, 1, 0, 0},
      {1, 0, 0, 0, 0, 1, 0, 0, 0, 1}, {1, 0, 1, 1, 1, 1, 0, 1, 1, 1},
      {1, 1, 1, 0, 0, 0, 1, 0, 0, 1}};

  Pair src = make_pair(8, 0);
  Pair dest = make_pair(0, 0);

  aStarSearch(grid, src, dest);

  return 0;
}
