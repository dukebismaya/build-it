/*
 * Program Description:
 * Longest Palindromic Subsequence (LPS) using Dynamic Programming.
 * Given a string S, finds the longest sequence of characters that forms a palindrome
 * where characters do not need to be contiguous.
 *
 * DP State Definition:
 * dp[i][j] = length of LPS in substring S[i..j]
 * - Base cases: dp[i][i] = 1 for all i
 * - Transitions:
 *   If S[i] == S[j]: dp[i][j] = 2 + dp[i+1][j-1]
 *   If S[i] != S[j]: dp[i][j] = max(dp[i+1][j], dp[i][j-1])
 *
 * Subsequence Reconstruction:
 * Traces back through the DP table to rebuild the actual palindromic string.
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(N^2)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct LPSResult {
    int length;
    std::string palindrome;
};

LPSResult longestPalindromicSubsequence(const std::string& s) {
    int n = s.length();
    if (n == 0) return {0, ""};

    std::vector<std::vector<int>> dp(n, std::vector<int>(n, 0));

    // Length 1 substrings
    for (int i = 0; i < n; ++i) {
        dp[i][i] = 1;
    }

    // Lengths 2 through n
    for (int len = 2; len <= n; ++len) {
        for (int i = 0; i <= n - len; ++i) {
            int j = i + len - 1;
            if (s[i] == s[j]) {
                dp[i][j] = 2 + (len == 2 ? 0 : dp[i + 1][j - 1]);
            } else {
                dp[i][j] = std::max(dp[i + 1][j], dp[i][j - 1]);
            }
        }
    }

    // Reconstruction of the palindrome string
    int i = 0, j = n - 1;
    std::string leftHalf = "";
    std::string middle = "";

    while (i <= j) {
        if (i == j) {
            middle += s[i];
            break;
        }
        if (s[i] == s[j]) {
            leftHalf += s[i];
            i++;
            j--;
        } else if (dp[i + 1][j] >= dp[i][j - 1]) {
            i++;
        } else {
            j--;
        }
    }

    std::string rightHalf = leftHalf;
    std::reverse(rightHalf.begin(), rightHalf.end());

    std::string fullPalindrome = leftHalf + middle + rightHalf;
    return {dp[0][n - 1], fullPalindrome};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Longest Palindromic Subsequence (DP)    \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> testCases = {
        "bbbab",
        "cbbd",
        "character",
        "agbdba",
        "turboventilator"
    };

    for (const auto& str : testCases) {
        auto [len, lps] = longestPalindromicSubsequence(str);
        std::cout << "Original String : \"" << str << "\" (length " << str.length() << ")\n";
        std::cout << "LPS Length      : " << len << "\n";
        std::cout << "LPS String      : \"" << lps << "\"\n";
        std::cout << "--------------------------------------------------------\n";
    }

    return 0;
}
