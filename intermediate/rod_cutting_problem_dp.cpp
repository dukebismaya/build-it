/*
 * Program Description:
 * Rod Cutting Problem using Dynamic Programming.
 * Given a rod of length N inches and a table of prices for each length i (1 <= i <= N),
 * determine the maximum revenue obtainable by cutting up the rod and selling the pieces.
 *
 * DP Formulation (Unbounded Knapsack):
 * Let dp[i] be the maximum revenue for a rod of length i:
 * dp[i] = max_{1 <= j <= i} ( price[j - 1] + dp[i - j] )
 *
 * Cut Reconstruction:
 * cutChoice[i] stores the size of the first cut j that achieved the optimal revenue for length i.
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

struct RodCuttingResult {
    int maxRevenue;
    std::vector<int> cuts; // Lengths of cut pieces
};

RodCuttingResult cutRodDP(const std::vector<int>& prices, int n) {
    if (n <= 0) return {0, {}};

    std::vector<int> dp(n + 1, 0);
    std::vector<int> cutChoice(n + 1, 0);

    for (int i = 1; i <= n; ++i) {
        int maxVal = INT_MIN;
        int bestCut = -1;

        for (int j = 1; j <= i; ++j) {
            int currentVal = prices[j - 1] + dp[i - j];
            if (currentVal > maxVal) {
                maxVal = currentVal;
                bestCut = j;
            }
        }

        dp[i] = maxVal;
        cutChoice[i] = bestCut;
    }

    // Reconstruct cuts
    std::vector<int> pieces;
    int remaining = n;
    while (remaining > 0) {
        int piece = cutChoice[remaining];
        pieces.push_back(piece);
        remaining -= piece;
    }

    return {dp[n], pieces};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Rod Cutting Problem (DP Suite)      \n";
    std::cout << "==========================================\n\n";

    // Prices for rod lengths 1 to 8
    // Length: 1  2  3  4  5   6   7   8
    // Price : 1  5  8  9  10  17  17  20
    std::vector<int> prices = {1, 5, 8, 9, 10, 17, 17, 20};

    std::cout << "Price Chart:\n";
    std::cout << "  Length |";
    for (size_t i = 0; i < prices.size(); ++i) std::cout << " " << (i + 1) << " |";
    std::cout << "\n  Price  |";
    for (int p : prices) std::cout << " " << p << " |";
    std::cout << "\n\n";

    for (int rodLen = 1; rodLen <= 8; ++rodLen) {
        auto res = cutRodDP(prices, rodLen);
        std::cout << "Rod Length: " << rodLen << " -> Max Revenue: $" << res.maxRevenue << "\n";
        std::cout << "  Cut Pieces: ";
        for (int c : res.cuts) std::cout << c << "\" ";
        std::cout << "\n";
    }

    return 0;
}
