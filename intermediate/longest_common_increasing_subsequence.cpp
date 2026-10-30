/**
 * @file longest_common_increasing_subsequence.cpp
 * @brief Computes the Longest Common Increasing Subsequence (LCIS) of two sequences.
 *
 * Problem:
 * Given two sequences A of length N and B of length M, find the length and elements
 * of the longest sequence that is simultaneously:
 * 1. A subsequence of A
 * 2. A subsequence of B
 * 3. Strictly increasing
 *
 * Algorithm:
 * - Let dp[j] be the length of the LCIS ending at element B[j].
 * - Outer loop: iterate i from 0 to N - 1 (considering prefix A[0..i]).
 * - Inner loop: iterate j from 0 to M - 1:
 *   - Maintain `currentMax` = max(dp[k]) for all k < j such that B[k] < A[i], and `bestIdx` = k.
 *   - If A[i] == B[j]:
 *       if currentMax + 1 > dp[j]:
 *           dp[j] = currentMax + 1
 *           parent[j] = bestIdx
 *   - If A[i] > B[j]:
 *       if dp[j] > currentMax:
 *           currentMax = dp[j]
 *           bestIdx = j
 * - Track parent pointers to reconstruct the exact optimal subsequence.
 *
 * Time Complexity: O(N * M)
 * Space Complexity: O(M) auxiliary space
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct LCISResult {
    int length;
    std::vector<int> sequence;
};

/**
 * @brief Computes the Longest Common Increasing Subsequence (LCIS) of two integer sequences.
 * @param a First sequence.
 * @param b Second sequence.
 * @return LCISResult containing max length and the reconstructed sequence.
 */
LCISResult findLCIS(const std::vector<int>& a, const std::vector<int>& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());

    if (n == 0 || m == 0) {
        return {0, {}};
    }

    std::vector<int> dp(m, 0);
    std::vector<int> parent(m, -1);

    for (int i = 0; i < n; ++i) {
        int currentMax = 0;
        int bestIdx = -1;

        for (int j = 0; j < m; ++j) {
            if (a[i] == b[j]) {
                if (currentMax + 1 > dp[j]) {
                    dp[j] = currentMax + 1;
                    parent[j] = bestIdx;
                }
            } else if (a[i] > b[j]) {
                if (dp[j] > currentMax) {
                    currentMax = dp[j];
                    bestIdx = j;
                }
            }
        }
    }

    // Find the overall maximum in dp
    int maxLen = 0;
    int endIdx = -1;
    for (int j = 0; j < m; ++j) {
        if (dp[j] > maxLen) {
            maxLen = dp[j];
            endIdx = j;
        }
    }

    if (maxLen == 0) {
        return {0, {}};
    }

    // Reconstruct the sequence
    std::vector<int> lcis;
    int curr = endIdx;
    while (curr != -1) {
        lcis.push_back(b[curr]);
        curr = parent[curr];
    }
    std::reverse(lcis.begin(), lcis.end());

    return {maxLen, lcis};
}

void printSequence(const std::string& label, const std::vector<int>& seq) {
    std::cout << label << " [ ";
    for (int val : seq) {
        std::cout << val << " ";
    }
    std::cout << "] (size " << seq.size() << ")\n";
}

int main() {
    std::cout << "=== Longest Common Increasing Subsequence (LCIS) ===\n\n";

    // Example 1
    std::vector<int> a1 = {3, 4, 9, 1, 2, 8};
    std::vector<int> b1 = {5, 3, 8, 4, 1, 2, 8};

    printSequence("Sequence A1:", a1);
    printSequence("Sequence B1:", b1);

    auto res1 = findLCIS(a1, b1);
    std::cout << "LCIS Length: " << res1.length << '\n';
    printSequence("LCIS:", res1.sequence);
    std::cout << '\n';

    // Example 2
    std::vector<int> a2 = {1, 3, 5, 2, 7, 8};
    std::vector<int> b2 = {1, 2, 3, 5, 8};

    printSequence("Sequence A2:", a2);
    printSequence("Sequence B2:", b2);

    auto res2 = findLCIS(a2, b2);
    std::cout << "LCIS Length: " << res2.length << '\n';
    printSequence("LCIS:", res2.sequence);
    std::cout << '\n';

    // Example 3: Disjoint sequences
    std::vector<int> a3 = {10, 20, 30};
    std::vector<int> b3 = {40, 50, 60};

    printSequence("Sequence A3:", a3);
    printSequence("Sequence B3:", b3);

    auto res3 = findLCIS(a3, b3);
    std::cout << "LCIS Length: " << res3.length << '\n';
    printSequence("LCIS:", res3.sequence);

    return 0;
}
