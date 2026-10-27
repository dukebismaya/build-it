/**
 * @file longest_increasing_path_in_matrix.cpp
 * @brief Finds the longest strictly increasing path in a 2D matrix using DFS with memoization.
 *
 * Algorithm:
 * - A path can move in 4 cardinal directions (up, down, left, right).
 * - Since values must be strictly increasing, there cannot be any cycles in the state transitions.
 *   Hence, the state graph is a Directed Acyclic Graph (DAG).
 * - We can compute the longest path from any cell (r, c) using depth-first search with memoization (DP).
 * - Overall longest path is max over all cells of longestPath(r, c).
 * - Also reconstructs and prints the exact coordinate and value path.
 *
 * Time Complexity: O(R * C) where R is row count and C is column count (each cell is visited once).
 * Space Complexity: O(R * C) for the memoization table and recursion stack.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Cell {
    int r;
    int c;
};

class LongestIncreasingPathFinder {
private:
    int rows;
    int cols;
    const std::vector<std::vector<int>>& grid;
    std::vector<std::vector<int>> memo;
    std::vector<std::vector<Cell>> nextStep;

    const int dr[4] = {-1, 1, 0, 0};
    const int dc[4] = {0, 0, -1, 1};

    int dfs(int r, int c) {
        if (memo[r][c] != 0) {
            return memo[r][c];
        }

        int maxLen = 1;
        Cell bestNext = {-1, -1};

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
                if (grid[nr][nc] > grid[r][c]) {
                    int len = 1 + dfs(nr, nc);
                    if (len > maxLen) {
                        maxLen = len;
                        bestNext = {nr, nc};
                    }
                }
            }
        }

        nextStep[r][c] = bestNext;
        return memo[r][c] = maxLen;
    }

public:
    LongestIncreasingPathFinder(const std::vector<std::vector<int>>& matrix)
        : rows(static_cast<int>(matrix.size())),
          cols(matrix.empty() ? 0 : static_cast<int>(matrix[0].size())),
          grid(matrix),
          memo(rows, std::vector<int>(cols, 0)),
          nextStep(rows, std::vector<Cell>(cols, {-1, -1})) {}

    int findLongestPath(std::vector<Cell>& reconstructedPath) {
        if (rows == 0 || cols == 0) return 0;

        int overallMax = 0;
        Cell startCell = {0, 0};

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                int len = dfs(r, c);
                if (len > overallMax) {
                    overallMax = len;
                    startCell = {r, c};
                }
            }
        }

        // Reconstruct path
        reconstructedPath.clear();
        Cell curr = startCell;
        while (curr.r != -1 && curr.c != -1) {
            reconstructedPath.push_back(curr);
            curr = nextStep[curr.r][curr.c];
        }

        return overallMax;
    }
};

void printGrid(const std::vector<std::vector<int>>& grid) {
    for (const auto& row : grid) {
        std::cout << "  ";
        for (int val : row) {
            std::cout << std::setw(4) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Longest Increasing Path in Matrix (DFS + Memoization) ===\n\n";

    std::vector<std::vector<int>> grid1 = {
        {9, 9, 4},
        {6, 6, 8},
        {2, 1, 1}
    };

    std::cout << "Matrix 1:\n";
    printGrid(grid1);

    LongestIncreasingPathFinder finder1(grid1);
    std::vector<Cell> path1;
    int maxLen1 = finder1.findLongestPath(path1);

    std::cout << "Longest Increasing Path Length: " << maxLen1 << '\n';
    std::cout << "Path Sequence: ";
    for (size_t i = 0; i < path1.size(); ++i) {
        std::cout << grid1[path1[i].r][path1[i].c] 
                  << " (" << path1[i].r << "," << path1[i].c << ")";
        if (i + 1 < path1.size()) std::cout << " -> ";
    }
    std::cout << "\n\n";

    std::vector<std::vector<int>> grid2 = {
        {3, 4, 5},
        {3, 2, 6},
        {2, 2, 1}
    };

    std::cout << "Matrix 2:\n";
    printGrid(grid2);

    LongestIncreasingPathFinder finder2(grid2);
    std::vector<Cell> path2;
    int maxLen2 = finder2.findLongestPath(path2);

    std::cout << "Longest Increasing Path Length: " << maxLen2 << '\n';
    std::cout << "Path Sequence: ";
    for (size_t i = 0; i < path2.size(); ++i) {
        std::cout << grid2[path2[i].r][path2[i].c] 
                  << " (" << path2[i].r << "," << path2[i].c << ")";
        if (i + 1 < path2.size()) std::cout << " -> ";
    }
    std::cout << '\n';

    return 0;
}
