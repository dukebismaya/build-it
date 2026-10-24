/*
 * Program Description:
 * Longest Common Substring (LCS) using Dynamic Programming.
 * Given two strings S1 and S2, finds the longest string that is a CONTIGUOUS
 * substring of both.
 *
 * Distinct from Longest Common Subsequence:
 * - Subsequence: characters do not have to be consecutive (e.g. "ace" in "abcde").
 * - Substring: characters MUST be strictly consecutive (e.g. "cde" in "abcde").
 *
 * DP State Definition:
 * dp[i][j] = length of the longest common suffix of prefixes S1[0..i-1] and S2[0..j-1]
 * - If S1[i-1] == S2[j-1]: dp[i][j] = dp[i-1][j-1] + 1
 * - If S1[i-1] != S2[j-1]: dp[i][j] = 0 (reset because contiguous matching is broken)
 *
 * Time Complexity: O(M * N)
 * Space Complexity: O(M * N)
 */

#include <iostream>
#include <string>
#include <vector>

struct LCSubstrResult {
    int length;
    std::string substring;
    int startIndexS1;
    int startIndexS2;
};

LCSubstrResult findLongestCommonSubstring(const std::string& s1, const std::string& s2) {
    int m = s1.length();
    int n = s2.length();

    if (m == 0 || n == 0) return {0, "", -1, -1};

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    int maxLen = 0;
    int endIdxS1 = -1;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > maxLen) {
                    maxLen = dp[i][j];
                    endIdxS1 = i - 1;
                }
            } else {
                dp[i][j] = 0;
            }
        }
    }

    if (maxLen == 0) {
        return {0, "", -1, -1};
    }

    int startS1 = endIdxS1 - maxLen + 1;
    std::string resultStr = s1.substr(startS1, maxLen);
    int startS2 = s2.find(resultStr);

    return {maxLen, resultStr, startS1, startS2};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Longest Common Substring (DP)        \n";
    std::cout << "==========================================\n\n";

    std::vector<std::pair<std::string, std::string>> testCases = {
        {"OldSite:GeeksforGeeks.org", "NewSite:GeeksQuiz.com"},
        {"photograph", "tomography"},
        {"algorithm", "altruistic"},
        {"abcdef", "xyz"}
    };

    for (const auto& [s1, s2] : testCases) {
        auto res = findLongestCommonSubstring(s1, s2);
        std::cout << "String 1: \"" << s1 << "\"\n";
        std::cout << "String 2: \"" << s2 << "\"\n";
        if (res.length > 0) {
            std::cout << "  Longest Common Substring: \"" << res.substring
                      << "\" (length " << res.length << ")\n";
            std::cout << "  Indices: S1[" << res.startIndexS1 << "], S2[" << res.startIndexS2 << "]\n";
        } else {
            std::cout << "  [No common substring found]\n";
        }
        std::cout << "--------------------------------------------------------\n";
    }

    return 0;
}
