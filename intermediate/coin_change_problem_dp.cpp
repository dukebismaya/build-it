/*
 * Program Description:
 * Coin Change Problem Suite using Dynamic Programming.
 *
 * Implements two classic DP formulations:
 * 1. Minimum Coins to make amount (Optimization):
 *    Finds the fewest coins that you need to make up that amount, and reconstructs
 *    the exact set of coins chosen.
 *    Shows why Greedy fails (e.g., coins {1, 3, 4} for amount 6: Greedy gives 4+1+1 = 3 coins,
 *    whereas DP finds 3+3 = 2 coins).
 * 2. Number of Unique Combinations (Counting / Unbounded Knapsack):
 *    Computes how many distinct combinations of coins sum to the target amount.
 *
 * Time Complexity: O(N * Amount) where N is number of coin denominations.
 * Space Complexity: O(Amount).
 */

#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>

struct MinCoinsResult {
    int minCoins;
    std::vector<int> usedCoins;
};

// Part 1: Minimum coins with combination recovery
MinCoinsResult minCoinsToMakeChange(const std::vector<int>& coins, int amount) {
    if (amount <= 0) return {0, {}};

    std::vector<int> dp(amount + 1, INT_MAX);
    std::vector<int> parent(amount + 1, -1);
    dp[0] = 0;

    for (int i = 1; i <= amount; ++i) {
        for (int c : coins) {
            if (i >= c && dp[i - c] != INT_MAX) {
                if (dp[i - c] + 1 < dp[i]) {
                    dp[i] = dp[i - c] + 1;
                    parent[i] = c;
                }
            }
        }
    }

    if (dp[amount] == INT_MAX) {
        return {-1, {}}; // Impossible to make change
    }

    // Reconstruct coin choices
    std::vector<int> chosen;
    int curr = amount;
    while (curr > 0) {
        int coin = parent[curr];
        chosen.push_back(coin);
        curr -= coin;
    }

    return {dp[amount], chosen};
}

// Part 2: Number of unique combinations (order does not matter)
long long countCoinChangeWays(const std::vector<int>& coins, int amount) {
    std::vector<long long> dp(amount + 1, 0);
    dp[0] = 1;

    for (int c : coins) {
        for (int i = c; i <= amount; ++i) {
            dp[i] += dp[i - c];
        }
    }

    return dp[amount];
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Coin Change Dynamic Programming      \n";
    std::cout << "==========================================\n\n";

    // Counterexample where Greedy algorithm fails:
    std::vector<int> coins1 = {1, 3, 4};
    int amount1 = 6;

    std::cout << "Coins: [1, 3, 4], Target Amount: " << amount1 << "\n";
    auto res1 = minCoinsToMakeChange(coins1, amount1);
    std::cout << "  Optimal Min Coins: " << res1.minCoins << "\n";
    std::cout << "  Coins Selected   : ";
    for (int c : res1.usedCoins) std::cout << c << " ";
    std::cout << "(Greedy would pick: 4 + 1 + 1 = 3 coins)\n\n";

    long long ways1 = countCoinChangeWays(coins1, amount1);
    std::cout << "  Total distinct combinations: " << ways1 << " ways\n\n";

    // Standard currency denominations
    std::vector<int> coins2 = {1, 2, 5, 10, 20, 50};
    int amount2 = 47;

    std::cout << "------------------------------------------\n";
    std::cout << "Standard Currency: [1, 2, 5, 10, 20, 50], Amount: " << amount2 << "\n";
    auto res2 = minCoinsToMakeChange(coins2, amount2);
    std::cout << "  Optimal Min Coins: " << res2.minCoins << "\n";
    std::cout << "  Coins Selected   : ";
    for (int c : res2.usedCoins) std::cout << c << " ";
    std::cout << "\n";

    long long ways2 = countCoinChangeWays(coins2, amount2);
    std::cout << "  Total distinct combinations: " << ways2 << " ways\n";

    return 0;
}
