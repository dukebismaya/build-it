/**
 * @file hungarian_algorithm_assignment.cpp
 * @brief Solves the Minimum Cost Bipartite Matching (Assignment Problem) using the Hungarian Algorithm.
 *
 * Problem:
 * Given an N x M cost matrix (N <= M) representing the cost of assigning worker i to job j,
 * find an assignment of each worker to a unique job such that the total cost is minimized.
 *
 * Algorithm (Kuhn-Munkres / Hungarian Algorithm with Potentials):
 * - Maintains dual potentials u[i] for workers and v[j] for jobs.
 * - For each worker 1..N, finds an augmenting path using Dijkstra-like slack updates:
 *   slack[j] = min_{i in visited} (cost[i][j] - u[i] - v[j]).
 * - Augments along the shortest alternating path and updates dual potentials.
 * - Guarantees exact global minimum cost assignment.
 *
 * Time Complexity: O(N^2 * M) <= O(N^3) for square N x N matrices.
 * Space Complexity: O(N * M) for the matrix and O(M) auxiliary arrays.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <limits>
#include <algorithm>

class HungarianAlgorithm {
public:
    static constexpr long long INF = std::numeric_limits<long long>::max() / 4;

    struct Result {
        long long minCost;
        std::vector<int> assignment; // assignment[i] is the job (0-indexed) assigned to worker i (0-indexed)
    };

    /**
     * @brief Solves the assignment problem for minimum total cost.
     * @param costMatrix Matrix of dimensions N x M where costMatrix[i][j] is the cost of assigning worker i to job j.
     *                   Requirement: N <= M.
     * @return Result containing minimum cost and the job assigned to each worker.
     */
    static Result solve(const std::vector<std::vector<long long>>& costMatrix) {
        int n = static_cast<int>(costMatrix.size());
        if (n == 0) return {0, {}};
        int m = static_cast<int>(costMatrix[0].size());

        // We use 1-indexed arrays for standard potential implementation
        // cost 1..n to 1..m
        std::vector<long long> u(n + 1, 0);
        std::vector<long long> v(m + 1, 0);
        std::vector<int> p(m + 1, 0);       // p[j] = worker assigned to job j
        std::vector<int> way(m + 1, 0);     // previous job in alternating path

        for (int i = 1; i <= n; ++i) {
            p[0] = i;
            int j0 = 0;
            std::vector<long long> minv(m + 1, INF);
            std::vector<bool> used(m + 1, false);

            do {
                used[j0] = true;
                int i0 = p[j0];
                long long delta = INF;
                int j1 = 0;

                for (int j = 1; j <= m; ++j) {
                    if (!used[j]) {
                        long long cur = costMatrix[i0 - 1][j - 1] - u[i0] - v[j];
                        if (cur < minv[j]) {
                            minv[j] = cur;
                            way[j] = j0;
                        }
                        if (minv[j] < delta) {
                            delta = minv[j];
                            j1 = j;
                        }
                    }
                }

                for (int j = 0; j <= m; ++j) {
                    if (used[j]) {
                        u[p[j]] += delta;
                        v[j] -= delta;
                    } else {
                        minv[j] -= delta;
                    }
                }

                j0 = j1;
            } while (p[j0] != 0);

            // Augment along alternating path
            do {
                int j1 = way[j0];
                p[j0] = p[j1];
                j0 = j1;
            } while (j0 != 0);
        }

        Result res;
        res.minCost = -v[0]; // Total optimal cost equals -v[0]
        res.assignment.assign(n, -1);

        for (int j = 1; j <= m; ++j) {
            if (p[j] > 0 && p[j] <= n) {
                res.assignment[p[j] - 1] = j - 1; // 0-indexed
            }
        }

        return res;
    }
};

void printCostMatrix(const std::vector<std::vector<long long>>& matrix) {
    int n = static_cast<int>(matrix.size());
    int m = static_cast<int>(matrix[0].size());

    std::cout << "       ";
    for (int j = 0; j < m; ++j) {
        std::cout << "Job " << std::setw(2) << j << " ";
    }
    std::cout << "\n      " << std::string(m * 7 + 1, '-') << '\n';

    for (int i = 0; i < n; ++i) {
        std::cout << "Wkr " << std::setw(2) << i << "|";
        for (int j = 0; j < m; ++j) {
            std::cout << std::setw(6) << matrix[i][j] << " ";
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Hungarian Algorithm (Minimum Cost Bipartite Assignment) ===\n\n";

    // Test Case 1: Standard 4x4 Assignment Matrix
    std::vector<std::vector<long long>> cost1 = {
        {9, 11, 14, 17},
        {6, 15, 13, 20},
        {13, 13, 10, 11},
        {17, 10, 12, 16}
    };

    std::cout << "Example 1: 4 Workers to 4 Jobs\n";
    printCostMatrix(cost1);

    auto result1 = HungarianAlgorithm::solve(cost1);
    std::cout << "\nOptimal Assignment:\n";
    for (size_t i = 0; i < result1.assignment.size(); ++i) {
        int job = result1.assignment[i];
        std::cout << "  Worker " << i << " -> Job " << job 
                  << " (Cost: " << cost1[i][job] << ")\n";
    }
    std::cout << "Minimum Total Cost: " << result1.minCost << "\n\n";

    // Test Case 2: Rectangular matrix (3 workers, 5 jobs)
    std::vector<std::vector<long long>> cost2 = {
        {10, 5, 13, 15, 16},
        {3,  9, 18, 13,  6},
        {10, 7,  2,  2,  2}
    };

    std::cout << "Example 2: Rectangular Matrix (3 Workers to 5 Jobs)\n";
    printCostMatrix(cost2);

    auto result2 = HungarianAlgorithm::solve(cost2);
    std::cout << "\nOptimal Assignment:\n";
    for (size_t i = 0; i < result2.assignment.size(); ++i) {
        int job = result2.assignment[i];
        std::cout << "  Worker " << i << " -> Job " << job 
                  << " (Cost: " << cost2[i][job] << ")\n";
    }
    std::cout << "Minimum Total Cost: " << result2.minCost << '\n';

    return 0;
}
