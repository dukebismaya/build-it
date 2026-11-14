/**
 * @file divide_and_conquer_dp_optimization.cpp
 * @brief Divide-and-Conquer Dynamic Programming Optimization in C++17.
 *
 * Overview:
 *   Accelerates 2D dynamic programming recurrences of the form:
 *     dp[i][j] = min_{k < j} { dp[i - 1][k] + cost(k + 1, j) }
 *   from standard O(K * N^2) time down to O(K * N log N).
 *
 * Monotonicity Condition:
 *   Applies when the optimal split point opt[i][j] is monotonic in j:
 *     opt[i][j] <= opt[i][j + 1]
 *   which is guaranteed whenever the cost function satisfies the Monge property
 *   (quadrangle inequality):
 *     cost(a, c) + cost(b, d) <= cost(a, d) + cost(b, c)   for all a <= b <= c <= d.
 *
 * Divide and Conquer Algorithm for Layer i:
 *   compute(L, R, optL, optR):
 *     1. Let mid = (L + R) / 2.
 *     2. Find opt[mid] by searching k in [optL, min(mid - 1, optR)].
 *     3. Recurse left:  compute(L, mid - 1, optL, opt[mid])
 *     4. Recurse right: compute(mid + 1, R, opt[mid], optR)
 *   Each recursion level does at most O(N) work, with O(log N) depth -> O(N log N) per layer.
 *
 * Problem Application:
 *   Partitioning N points into K contiguous clusters to minimize sum of squared deviations.
 *
 * Complexity:
 *   - Time: O(K * N log N)
 *   - Space: O(K * N) or O(N) using row-rolling.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <numeric>
#include <limits>

namespace dp {

class DivideAndConquerDP {
private:
    int n;
    int kPartitions;
    std::vector<double> arr;
    std::vector<double> pref;
    std::vector<double> prefSq;

    // Cost of putting arr[i..j] (1-based) into a single cluster: sum (x - mean)^2
    // = sum(x^2) - (sum(x))^2 / len
    double cost(int i, int j) const {
        if (i > j) return 0.0;
        double sumX = pref[j] - pref[i - 1];
        double sumX2 = prefSq[j] - prefSq[i - 1];
        double len = static_cast<double>(j - i + 1);
        return sumX2 - (sumX * sumX) / len;
    }

    void computeLayer(int l, int r, int optL, int optR,
                      const std::vector<double>& prevDP,
                      std::vector<double>& currDP) const {
        if (l > r) return;

        int mid = (l + r) / 2;
        int bestK = optL;
        double bestVal = std::numeric_limits<double>::infinity();

        int maxK = std::min(mid - 1, optR);
        for (int k = optL; k <= maxK; ++k) {
            double candidate = prevDP[k] + cost(k + 1, mid);
            if (candidate < bestVal) {
                bestVal = candidate;
                bestK = k;
            }
        }

        currDP[mid] = bestVal;

        // Recurse left and right
        computeLayer(l, mid - 1, optL, bestK, prevDP, currDP);
        computeLayer(mid + 1, r, bestK, optR, prevDP, currDP);
    }

public:
    DivideAndConquerDP(std::vector<double> data, int k)
        : n(static_cast<int>(data.size())), kPartitions(k), arr(std::move(data)),
          pref(n + 1, 0.0), prefSq(n + 1, 0.0) {
        for (int i = 0; i < n; ++i) {
            pref[i + 1] = pref[i] + arr[i];
            prefSq[i + 1] = prefSq[i] + arr[i] * arr[i];
        }
    }

    double solve() const {
        const double INF = std::numeric_limits<double>::infinity();
        std::vector<double> prevDP(n + 1, INF);
        std::vector<double> currDP(n + 1, INF);

        // Base case: 1 partition
        prevDP[0] = 0.0;
        for (int j = 1; j <= n; ++j) {
            prevDP[j] = cost(1, j);
        }

        // Compute for partitions 2 to kPartitions
        for (int p = 2; p <= kPartitions; ++p) {
            currDP[0] = 0.0;
            computeLayer(1, n, 0, n, prevDP, currDP);
            prevDP = currDP;
        }

        return prevDP[n];
    }
};

} // namespace dp

int main() {
    std::cout << "========================================\n";
    std::cout << "   DIVIDE & CONQUER DP OPTIMIZATION     \n";
    std::cout << "  (O(K * N log N) 1D Cluster Partition) \n";
    std::cout << "========================================\n\n";

    // 10 sorted points to be partitioned into K = 3 optimal contiguous clusters
    std::vector<double> points = {1.0, 2.0, 3.0, 10.0, 11.0, 12.0, 13.0, 25.0, 26.0, 27.0};
    int K = 3;

    std::cout << "Input Points (" << points.size() << " total):\n  [ ";
    for (double x : points) std::cout << std::fixed << std::setprecision(1) << x << " ";
    std::cout << "]\n";
    std::cout << "Target Partitions (Clusters) K = " << K << "\n\n";

    dp::DivideAndConquerDP solver(points, K);
    double minCost = solver.solve();

    std::cout << "Optimization Results:\n";
    std::cout << "  - Minimum Total Cluster Variance: " << std::fixed << std::setprecision(4)
              << minCost << "\n";
    std::cout << "  - Theoretical partition: {1, 2, 3}, {10, 11, 12, 13}, {25, 26, 27}\n";
    std::cout << "  - Time Complexity: O(K * N log N) vs Naive O(K * N^2)\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
