/**
 * @file edit_distance_levenshtein_dp.cpp
 * @brief Computes the Levenshtein Edit Distance between two strings with full operation reconstruction.
 *
 * Problem:
 * Given two strings s1 and s2, compute the minimum number of single-character operations
 * (Insertion, Deletion, or Substitution) required to transform s1 into s2.
 *
 * Dynamic Programming State:
 * dp[i][j] = minimum edit distance between s1[0..i-1] and s2[0..j-1]
 * Base cases:
 *   dp[i][0] = i (i deletions)
 *   dp[0][j] = j (j insertions)
 * Transitions:
 *   If s1[i-1] == s2[j-1]:
 *     dp[i][j] = dp[i-1][j-1] (Match, cost 0)
 *   Else:
 *     dp[i][j] = 1 + min({
 *       dp[i-1][j],   // Deletion from s1
 *       dp[i][j-1],   // Insertion into s1
 *       dp[i-1][j-1]  // Substitution in s1
 *     })
 *
 * Time Complexity: O(M * N) where M = len(s1), N = len(s2)
 * Space Complexity:
 * - O(M * N) with operation backtracking
 * - O(min(M, N)) space-optimized distance computation
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

enum class EditOpType {
    MATCH,
    SUBSTITUTE,
    INSERT,
    DELETE
};

struct EditOperation {
    EditOpType type;
    char fromChar;
    char toChar;
    int indexInSource;
};

struct EditDistanceResult {
    int distance;
    std::vector<EditOperation> operations;
};

/**
 * @brief Computes the edit distance and reconstructs the sequence of operations.
 */
EditDistanceResult computeEditDistance(const std::string& s1, const std::string& s2) {
    int m = static_cast<int>(s1.length());
    int n = static_cast<int>(s2.length());

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int j = 0; j <= n; ++j) dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({
                    dp[i - 1][j],     // Delete
                    dp[i][j - 1],     // Insert
                    dp[i - 1][j - 1]  // Substitute
                });
            }
        }
    }

    // Reconstruct edit operations by backtracking
    std::vector<EditOperation> ops;
    int i = m;
    int j = n;

    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && s1[i - 1] == s2[j - 1]) {
            ops.push_back({EditOpType::MATCH, s1[i - 1], s2[j - 1], i - 1});
            --i;
            --j;
        } else if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + 1) {
            ops.push_back({EditOpType::SUBSTITUTE, s1[i - 1], s2[j - 1], i - 1});
            --i;
            --j;
        } else if (j > 0 && dp[i][j] == dp[i][j - 1] + 1) {
            ops.push_back({EditOpType::INSERT, '\0', s2[j - 1], i});
            --j;
        } else if (i > 0 && dp[i][j] == dp[i - 1][j + 0] + 1) {
            ops.push_back({EditOpType::DELETE, s1[i - 1], '\0', i - 1});
            --i;
        }
    }

    std::reverse(ops.begin(), ops.end());
    return {dp[m][n], ops};
}

/**
 * @brief Computes edit distance using O(min(M, N)) auxiliary space.
 */
int computeEditDistanceSpaceOptimized(std::string s1, std::string s2) {
    if (s1.length() < s2.length()) {
        std::swap(s1, s2);
    }
    int m = static_cast<int>(s1.length());
    int n = static_cast<int>(s2.length());

    std::vector<int> prev(n + 1, 0);
    std::vector<int> curr(n + 1, 0);

    for (int j = 0; j <= n; ++j) prev[j] = j;

    for (int i = 1; i <= m; ++i) {
        curr[0] = i;
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                curr[j] = prev[j - 1];
            } else {
                curr[j] = 1 + std::min({prev[j], curr[j - 1], prev[j - 1]});
            }
        }
        prev = curr;
    }

    return prev[n];
}

void printOperations(const std::vector<EditOperation>& ops) {
    int step = 1;
    for (const auto& op : ops) {
        std::cout << "    Step " << std::setw(2) << step++ << ": ";
        switch (op.type) {
            case EditOpType::MATCH:
                std::cout << "MATCH      '" << op.fromChar << "'\n";
                break;
            case EditOpType::SUBSTITUTE:
                std::cout << "SUBSTITUTE '" << op.fromChar << "' -> '" << op.toChar << "'\n";
                break;
            case EditOpType::INSERT:
                std::cout << "INSERT     '" << op.toChar << "'\n";
                break;
            case EditOpType::DELETE:
                std::cout << "DELETE     '" << op.fromChar << "'\n";
                break;
        }
    }
}

int main() {
    std::cout << "=== Levenshtein Edit Distance (DP & Operation Reconstruction) ===\n\n";

    std::vector<std::pair<std::string, std::string>> testCases = {
        {"kitten", "sitting"},
        {"intention", "execution"},
        {"algorithm", "altruistic"}
    };

    for (const auto& [s1, s2] : testCases) {
        std::cout << "Source:      \"" << s1 << "\"\n";
        std::cout << "Target:      \"" << s2 << "\"\n";

        auto result = computeEditDistance(s1, s2);
        int spaceOpt = computeEditDistanceSpaceOptimized(s1, s2);

        std::cout << "Minimum Edit Distance: " << result.distance 
                  << " (Space-optimized check: " << spaceOpt << ")\n";
        std::cout << "Transformation Script:\n";
        printOperations(result.operations);
        std::cout << '\n';
    }

    return 0;
}
