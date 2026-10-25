/*
 * Program Description:
 * Longest Repeating Subsequence (LRS) using Dynamic Programming.
 * Given a string S, finds the longest subsequence that appears at least twice
 * in S, such that the two subsequences do not share the exact same character indices.
 *
 * Modified LCS Formulation:
 * Compute LCS(S, S) with the added restriction:
 * dp[i][j] = dp[i-1][j-1] + 1  ONLY IF  S[i-1] == S[j-1] AND i != j
 * Else dp[i][j] = max(dp[i-1][j], dp[i][j-1]).
 *
 * Backtracking:
 * Traces back through the DP matrix to extract the actual repeating subsequence.
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(N^2)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct LRSResult {
    int length;
    std::string subsequence;
};

LRSResult findLongestRepeatingSubsequence(const std::string& s) {
    int n = s.length();
    if (n <= 1) return {0, ""};

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s[i - 1] == s[j - 1] && i != j) {
                dp[i][j] = 1 + dp[i - 1][j - 1];
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    // Backtrack to recover string
    std::string lrs = "";
    int i = n, j = n;
    while (i > 0 && j > 0) {
        if (s[i - 1] == s[j - 1] && i != j) {
            lrs.push_back(s[i - 1]);
            i--;
            j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    std::reverse(lrs.begin(), lrs.end());
    return {dp[n][n], lrs};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Longest Repeating Subsequence (DP)     \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> testCases = {
        "AABEBCDD", // Expected: "ABD"
        "axxzxy",   // Expected: "x"
        "banana",   // Expected: "an" or "na"
        "abcdef",   // Expected: "" (all unique chars)
        "aabbcc"    // Expected: "abc"
    };

    for (const auto& str : testCases) {
        auto res = findLongestRepeatingSubsequence(str);
        std::cout << "Input String: \"" << str << "\"\n";
        std::cout << "  LRS Length: " << res.length << "\n";
        std::cout << "  LRS String: \"" << res.subsequence << "\"\n";
        std::cout << "------------------------------------------\n";
    }

    return 0;
}
