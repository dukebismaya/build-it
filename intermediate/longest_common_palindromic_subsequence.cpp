/**
 * @file longest_common_palindromic_subsequence.cpp
 * @brief Computes the Longest Common Palindromic Subsequence (LCPS) of two strings via memoized DP.
 *
 * Problem:
 * Given two strings S1 and S2, find the length and characters of the longest sequence
 * that is simultaneously:
 * 1. A subsequence of S1
 * 2. A subsequence of S2
 * 3. A palindrome (reads the same forwards and backwards)
 *
 * Dynamic Programming State:
 * Let solve(i, j, k, l) be the LCPS of S1[i..j] and S2[k..l].
 *
 * Transitions:
 * - Base case: if i > j or k > l, length is 0.
 * - If S1[i] == S1[j] && S2[k] == S2[l] && S1[i] == S2[k]:
 *     If (i == j || k == l): return 1
 *     Else: return 2 + solve(i + 1, j - 1, k + 1, l - 1)
 * - Else:
 *     return max({
 *         solve(i + 1, j, k, l),
 *         solve(i, j - 1, k, l),
 *         solve(i, j, k + 1, l),
 *         solve(i, j, k, l - 1)
 *     })
 *
 * Time Complexity: O(|S1|^2 * |S2|^2)
 * Space Complexity: O(|S1|^2 * |S2|^2)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

class LCPSFinder {
private:
    std::string s1;
    std::string s2;
    int n, m;
    std::vector<std::vector<std::vector<std::vector<int>>>> memo;

    int dp(int i, int j, int k, int l) {
        if (i > j || k > l) return 0;
        if (memo[i][j][k][l] != -1) return memo[i][j][k][l];

        if (s1[i] == s1[j] && s2[k] == s2[l] && s1[i] == s2[k]) {
            if (i == j || k == l) {
                return memo[i][j][k][l] = 1;
            }
            return memo[i][j][k][l] = 2 + dp(i + 1, j - 1, k + 1, l - 1);
        }

        int res = 0;
        res = std::max(res, dp(i + 1, j, k, l));
        res = std::max(res, dp(i, j - 1, k, l));
        res = std::max(res, dp(i, j, k + 1, l));
        res = std::max(res, dp(i, j, k, l - 1));

        return memo[i][j][k][l] = res;
    }

    std::string reconstruct(int i, int j, int k, int l) {
        if (i > j || k > l) return "";

        if (s1[i] == s1[j] && s2[k] == s2[l] && s1[i] == s2[k]) {
            if (i == j || k == l) {
                return std::string(1, s1[i]);
            }
            return s1[i] + reconstruct(i + 1, j - 1, k + 1, l - 1) + s1[i];
        }

        int best = dp(i, j, k, l);
        if (dp(i + 1, j, k, l) == best) return reconstruct(i + 1, j, k, l);
        if (dp(i, j - 1, k, l) == best) return reconstruct(i, j - 1, k, l);
        if (dp(i, j, k + 1, l) == best) return reconstruct(i, j, k + 1, l);
        return reconstruct(i, j, k, l - 1);
    }

public:
    LCPSFinder(std::string str1, std::string str2)
        : s1(std::move(str1)), s2(std::move(str2)),
          n(static_cast<int>(s1.length())), m(static_cast<int>(s2.length())),
          memo(n, std::vector<std::vector<std::vector<int>>>(
                      n, std::vector<std::vector<int>>(
                             m, std::vector<int>(m, -1)))) {}

    std::pair<int, std::string> solve() {
        if (n == 0 || m == 0) return {0, ""};
        int len = dp(0, n - 1, 0, m - 1);
        std::string palindrome = reconstruct(0, n - 1, 0, m - 1);
        return {len, palindrome};
    }
};

int main() {
    std::cout << "=== Longest Common Palindromic Subsequence (LCPS) ===\n\n";

    std::vector<std::pair<std::string, std::string>> testCases = {
        {"geeksforgeeks", "gkeesforgeeks"},
        {"eposide", "position"},
        {"banana", "anana"},
        {"abacaba", "bacab"}
    };

    for (const auto& [s1, s2] : testCases) {
        std::cout << "String 1: \"" << s1 << "\"\n";
        std::cout << "String 2: \"" << s2 << "\"\n";

        LCPSFinder finder(s1, s2);
        auto [length, lcpSubseq] = finder.solve();

        std::cout << "  LCPS Length:   " << length << '\n';
        std::cout << "  LCPS Sequence: \"" << lcpSubseq << "\"\n\n";
    }

    return 0;
}

