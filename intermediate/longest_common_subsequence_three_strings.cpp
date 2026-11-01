/**
 * @file longest_common_subsequence_three_strings.cpp
 * @brief Computes the Longest Common Subsequence (LCS) of three strings with path reconstruction.
 *
 * Problem:
 * Given three sequences A, B, and C of lengths N1, N2, and N3 respectively,
 * find the longest sequence that appears as a subsequence in all three.
 *
 * 3D Dynamic Programming Recurrence:
 * Let dp[i][j][k] = length of LCS of A[0..i-1], B[0..j-1], and C[0..k-1].
 *
 * Transitions:
 * If A[i-1] == B[j-1] && B[j-1] == C[k-1]:
 *     dp[i][j][k] = 1 + dp[i-1][j-1][k-1]
 * Else:
 *     dp[i][j][k] = max({
 *         dp[i-1][j][k],
 *         dp[i][j-1][k],
 *         dp[i][j][k-1]
 *     })
 *
 * Backtracking:
 * We can trace back from (N1, N2, N3) down to (0, 0, 0) to reconstruct the exact
 * common subsequence characters.
 *
 * Time Complexity: O(N1 * N2 * N3)
 * Space Complexity: O(N1 * N2 * N3)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

struct LCS3Result {
    int length;
    std::string subsequence;
};

/**
 * @brief Computes the LCS of three strings and reconstructs the sequence.
 */
LCS3Result findLCS3(const std::string& a, const std::string& b, const std::string& c) {
    int n1 = static_cast<int>(a.length());
    int n2 = static_cast<int>(b.length());
    int n3 = static_cast<int>(c.length());

    // 3D vector: dp[n1 + 1][n2 + 1][n3 + 1]
    std::vector<std::vector<std::vector<int>>> dp(
        n1 + 1, std::vector<std::vector<int>>(
            n2 + 1, std::vector<int>(n3 + 1, 0)
        )
    );

    for (int i = 1; i <= n1; ++i) {
        for (int j = 1; j <= n2; ++j) {
            for (int k = 1; k <= n3; ++k) {
                if (a[i - 1] == b[j - 1] && b[j - 1] == c[k - 1]) {
                    dp[i][j][k] = 1 + dp[i - 1][j - 1][k - 1];
                } else {
                    dp[i][j][k] = std::max({
                        dp[i - 1][j][k],
                        dp[i][j - 1][k],
                        dp[i][j][k - 1]
                    });
                }
            }
        }
    }

    // Backtrack to reconstruct the LCS string
    std::string lcsStr;
    int i = n1, j = n2, k = n3;

    while (i > 0 && j > 0 && k > 0) {
        if (a[i - 1] == b[j - 1] && b[j - 1] == c[k - 1]) {
            lcsStr.push_back(a[i - 1]);
            --i;
            --j;
            --k;
        } else if (dp[i - 1][j][k] >= dp[i][j - 1][k] && dp[i - 1][j][k] >= dp[i][j][k - 1]) {
            --i;
        } else if (dp[i][j - 1][k] >= dp[i - 1][j][k] && dp[i][j - 1][k] >= dp[i][j][k - 1]) {
            --j;
        } else {
            --k;
        }
    }

    std::reverse(lcsStr.begin(), lcsStr.end());
    return {dp[n1][n2][n3], lcsStr};
}

int main() {
    std::cout << "=== Longest Common Subsequence of Three Strings (3D DP) ===\n\n";

    // Test Case 1
    std::string s1 = "geeks";
    std::string s2 = "geeksfor";
    std::string s3 = "geeksforgeeks";

    std::cout << "Test 1:\n";
    std::cout << "  String 1: \"" << s1 << "\"\n";
    std::cout << "  String 2: \"" << s2 << "\"\n";
    std::cout << "  String 3: \"" << s3 << "\"\n";

    auto res1 = findLCS3(s1, s2, s3);
    std::cout << "  LCS Length: " << res1.length << '\n';
    std::cout << "  LCS String: \"" << res1.subsequence << "\"\n\n";

    // Test Case 2
    std::string a = "abcd1e2";
    std::string b = "bc12ea";
    std::string c = "bd1ea";

    std::cout << "Test 2:\n";
    std::cout << "  String 1: \"" << a << "\"\n";
    std::cout << "  String 2: \"" << b << "\"\n";
    std::cout << "  String 3: \"" << c << "\"\n";

    auto res2 = findLCS3(a, b, c);
    std::cout << "  LCS Length: " << res2.length << '\n';
    std::cout << "  LCS String: \"" << res2.subsequence << "\"\n\n";

    // Test Case 3: DNA sequence alignment snippet
    std::string dna1 = "AGGTAB";
    std::string dna2 = "GXTXAYB";
    std::string dna3 = "TGXAYB";

    std::cout << "Test 3 (Genomic Sequences):\n";
    std::cout << "  DNA 1: \"" << dna1 << "\"\n";
    std::cout << "  DNA 2: \"" << dna2 << "\"\n";
    std::cout << "  DNA 3: \"" << dna3 << "\"\n";

    auto res3 = findLCS3(dna1, dna2, dna3);
    std::cout << "  LCS Length: " << res3.length << '\n';
    std::cout << "  LCS String: \"" << res3.subsequence << "\"\n";

    return 0;
}

