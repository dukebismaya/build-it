/*
 * Program Description:
 * Egg Dropping Puzzle using Dynamic Programming.
 * Given K eggs and an N-floor building, determine the minimum number of drops
 * needed in the worst case to find the critical floor from which an egg breaks.
 *
 * DP State Definition:
 * dp[k][n] = minimum drops needed with k eggs and n floors.
 *
 * Base Cases:
 * - dp[1][n] = n (must test every floor sequentially from 1 to n)
 * - dp[k][0] = 0, dp[k][1] = 1
 *
 * Transition:
 * Dropping from floor x (1 <= x <= n):
 * 1. Egg breaks: Egg count decreases to k - 1, test floors below: dp[k - 1][x - 1]
 * 2. Egg survives: Egg count remains k, test floors above: dp[k][n - x]
 * Worst case for choice x: max(dp[k - 1][x - 1], dp[k][n - x])
 * Optimal choice: dp[k][n] = 1 + min_{1 <= x <= n} ( max(...) )
 *
 * Optimization:
 * Since dp[k-1][x-1] is strictly increasing with x, and dp[k][n-x] is strictly decreasing with x,
 * the optimal floor x can be found via Binary Search in O(K * N log N).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

int eggDropDP(int eggs, int floors) {
    std::vector<std::vector<int>> dp(eggs + 1, std::vector<int>(floors + 1, 0));

    // Base cases
    for (int i = 1; i <= eggs; ++i) {
        dp[i][1] = 1;
        dp[i][0] = 0;
    }
    for (int j = 1; j <= floors; ++j) {
        dp[1][j] = j;
    }

    for (int k = 2; k <= eggs; ++k) {
        for (int n = 2; n <= floors; ++n) {
            // Binary search for optimal drop floor x
            int low = 1, high = n;
            int minDrops = INT_MAX;

            while (low <= high) {
                int mid = low + (high - low) / 2;
                int broken = dp[k - 1][mid - 1];
                int unbroken = dp[k][n - mid];

                int worst = 1 + std::max(broken, unbroken);
                minDrops = std::min(minDrops, worst);

                if (broken < unbroken) {
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }

            dp[k][n] = minDrops;
        }
    }

    return dp[eggs][floors];
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Egg Dropping Puzzle (DP)          \n";
    std::cout << "==========================================\n\n";

    std::vector<std::pair<int, int>> tests = {
        {1, 10},  // 1 egg, 10 floors -> 10 drops
        {2, 10},  // 2 eggs, 10 floors -> 4 drops
        {2, 100}, // 2 eggs, 100 floors -> 14 drops (classic interview question!)
        {3, 14},  // 3 eggs, 14 floors -> 4 drops
        {4, 100}  // 4 eggs, 100 floors -> 8 drops
    };

    for (const auto& [eggs, floors] : tests) {
        int drops = eggDropDP(eggs, floors);
        std::cout << "Eggs: " << eggs << ", Floors: " << floors
                  << " -> Minimum Trials Needed: " << drops << "\n";
    }

    return 0;
}
