/*
 * Problem Description:
 * A* Search Algorithm.
 * An informed (best-first) search algorithm that finds the shortest path between a start and goal
 * node in a weighted graph. It combines Dijkstra's algorithm (using actual cost g(n)) with a
 * heuristic estimate h(n) of cost to goal, evaluating nodes using f(n) = g(n) + h(n).
 * Uses a priority queue to always expand the most promising node.
 * Guarantees optimal path when heuristic is admissible (never overestimates true cost).
 * Demonstrated on a 2D grid with obstacles using Manhattan distance heuristic.
 * Time Complexity: O(E log V) with binary heap, where E = edges, V = vertices.
 * Space Complexity: O(V) for open/closed sets.
 */

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>
#include <climits>

struct Cell {
    int row, col;
    int f, g, h;
    Cell* parent;

    Cell() : row(0), col(0), f(INT_MAX), g(INT_MAX), h(0), parent(nullptr) {}
};

struct CompareCell {
    bool operator()(const Cell* a, const Cell* b) const {
        return a->f > b->f;
    }
};

// Manhattan distance heuristic
int heuristic(int r1, int c1, int r2, int c2) {
    return std::abs(r1 - r2) + std::abs(c1 - c2);
}

bool isValid(int row, int col, int rows, int cols) {
    return row >= 0 && row < rows && col >= 0 && col < cols;
}

std::vector<std::pair<int, int>> aStarSearch(
    const std::vector<std::vector<int>>& grid,
    std::pair<int, int> start, std::pair<int, int> goal)
{
    int rows = grid.size();
    int cols = grid[0].size();

    // Direction vectors: up, down, left, right
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    std::vector<std::vector<Cell>> cells(rows, std::vector<Cell>(cols));
    std::vector<std::vector<bool>> closedList(rows, std::vector<bool>(cols, false));

    int sr = start.first, sc = start.second;
    int gr = goal.first, gc = goal.second;

    cells[sr][sc].row = sr;
    cells[sr][sc].col = sc;
    cells[sr][sc].g = 0;
    cells[sr][sc].h = heuristic(sr, sc, gr, gc);
    cells[sr][sc].f = cells[sr][sc].g + cells[sr][sc].h;

    std::priority_queue<Cell*, std::vector<Cell*>, CompareCell> openList;
    openList.push(&cells[sr][sc]);

    while (!openList.empty()) {
        Cell* current = openList.top();
        openList.pop();

        int r = current->row;
        int c = current->col;

        if (closedList[r][c]) continue;
        closedList[r][c] = true;

        if (r == gr && c == gc) {
            // Reconstruct path
            std::vector<std::pair<int, int>> path;
            Cell* node = &cells[gr][gc];
            while (node != nullptr) {
                path.push_back({node->row, node->col});
                node = node->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (isValid(nr, nc, rows, cols) && !closedList[nr][nc] && grid[nr][nc] == 0) {
                int newG = cells[r][c].g + 1;
                int newH = heuristic(nr, nc, gr, gc);
                int newF = newG + newH;

                if (newF < cells[nr][nc].f) {
                    cells[nr][nc].row = nr;
                    cells[nr][nc].col = nc;
                    cells[nr][nc].g = newG;
                    cells[nr][nc].h = newH;
                    cells[nr][nc].f = newF;
                    cells[nr][nc].parent = &cells[r][c];
                    openList.push(&cells[nr][nc]);
                }
            }
        }
    }

    return {}; // No path found
}

int main() {
    // 0 = passable, 1 = obstacle
    std::vector<std::vector<int>> grid = {
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 1, 1, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 1, 0, 0, 0},
        {0, 1, 0, 0, 0, 1, 1, 0},
        {0, 0, 0, 1, 0, 0, 0, 0}
    };

    std::pair<int, int> start = {0, 0};
    std::pair<int, int> goal = {5, 7};

    auto path = aStarSearch(grid, start, goal);

    std::cout << "A* Search Algorithm - Grid Pathfinding\n\n";
    if (path.empty()) {
        std::cout << "No path found!\n";
    } else {
        std::cout << "Path found (" << path.size() << " steps):\n";
        for (const auto& p : path) {
            std::cout << "(" << p.first << ", " << p.second << ") ";
        }
        std::cout << "\n\nGrid with path (marked as *):\n";
        std::vector<std::vector<char>> display(grid.size(), std::vector<char>(grid[0].size()));
        for (size_t i = 0; i < grid.size(); ++i)
            for (size_t j = 0; j < grid[0].size(); ++j)
                display[i][j] = grid[i][j] ? '#' : '.';
        for (const auto& p : path)
            display[p.first][p.second] = '*';
        display[start.first][start.second] = 'S';
        display[goal.first][goal.second] = 'G';
        for (const auto& row : display) {
            for (char c : row) std::cout << c << ' ';
            std::cout << '\n';
        }
    }

    return 0;
}
