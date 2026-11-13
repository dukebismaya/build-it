/**
 * @file hirschberg_linear_space_alignment.cpp
 * @brief Hirschberg's linear-space optimal sequence alignment algorithm in C++17.
 *
 * Overview (Dan Hirschberg, 1975):
 *   Standard Needleman-Wunsch dynamic programming finds the optimal global alignment
 *   between two strings of lengths M and N in O(M * N) time and O(M * N) space.
 *   For large biological sequences (e.g. genomic DNA with M, N ~ 100,000), quadratic
 *   memory is infeasible.
 *
 * Hirschberg's Algorithm:
 *   Combines divide-and-conquer with dynamic programming to compute the EXACT optimal
 *   alignment in:
 *     - Time: O(M * N)
 *     - Space: O(min(M, N))  [Linear Space!]
 *
 * Divide-and-Conquer Recurrence:
 *   1. Split string A at midpoint x_mid = M / 2 into A_left and A_right.
 *   2. Compute forward DP scores L1 between A_left and all prefixes of B in linear space.
 *   3. Compute reverse DP scores L2 between reversed A_right and all suffixes of B in linear space.
 *   4. Find optimal split point y_mid = argmax_j (L1[j] + L2[N - j]).
 *   5. Recursively solve (A_left, B[0..y_mid-1]) and (A_right, B[y_mid..N-1]),
 *      then concatenate their alignments.
 *
 * Scoring Model:
 *   Match: +2, Mismatch: -1, Gap: -2.
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace bio {

struct Scoring {
    int match = 2;
    int mismatch = -1;
    int gap = -2;
};

/**
 * @brief Computes the last row of the Needleman-Wunsch DP matrix in O(N) space.
 */
std::vector<int> getScoreRow(const std::string& a, const std::string& b, const Scoring& sc) {
    size_t n = b.length();
    std::vector<int> prev(n + 1, 0);
    std::vector<int> curr(n + 1, 0);

    for (size_t j = 0; j <= n; ++j) {
        prev[j] = static_cast<int>(j) * sc.gap;
    }

    for (size_t i = 1; i <= a.length(); ++i) {
        curr[0] = static_cast<int>(i) * sc.gap;
        for (size_t j = 1; j <= n; ++j) {
            int scoreSub = prev[j - 1] + (a[i - 1] == b[j - 1] ? sc.match : sc.mismatch);
            int scoreDel = prev[j] + sc.gap;
            int scoreIns = curr[j - 1] + sc.gap;
            curr[j] = std::max({scoreSub, scoreDel, scoreIns});
        }
        prev = curr;
    }

    return prev;
}

struct AlignmentResult {
    std::string alignedA;
    std::string alignedB;
    int score;
};

/**
 * @brief Base case standard Needleman-Wunsch for small subproblems.
 */
AlignmentResult needlemanWunschBase(const std::string& a, const std::string& b, const Scoring& sc) {
    size_t m = a.length();
    size_t n = b.length();

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (size_t i = 0; i <= m; ++i) dp[i][0] = static_cast<int>(i) * sc.gap;
    for (size_t j = 0; j <= n; ++j) dp[0][j] = static_cast<int>(j) * sc.gap;

    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            int matchScore = (a[i - 1] == b[j - 1]) ? sc.match : sc.mismatch;
            dp[i][j] = std::max({dp[i - 1][j - 1] + matchScore,
                                 dp[i - 1][j] + sc.gap,
                                 dp[i][j - 1] + sc.gap});
        }
    }

    // Backtrack alignment
    std::string outA, outB;
    size_t i = m, j = n;
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 &&
            dp[i][j] == dp[i - 1][j - 1] + (a[i - 1] == b[j - 1] ? sc.match : sc.mismatch)) {
            outA += a[i - 1];
            outB += b[j - 1];
            i--; j--;
        } else if (i > 0 && dp[i][j] == dp[i - 1][j] + sc.gap) {
            outA += a[i - 1];
            outB += '-';
            i--;
        } else {
            outA += '-';
            outB += b[j - 1];
            j--;
        }
    }

    std::reverse(outA.begin(), outA.end());
    std::reverse(outB.begin(), outB.end());
    return {outA, outB, dp[m][n]};
}

/**
 * @brief Recursive Hirschberg linear-space alignment.
 */
AlignmentResult hirschberg(const std::string& a, const std::string& b, const Scoring& sc) {
    size_t m = a.length();
    size_t n = b.length();

    // Base cases
    if (m == 0) {
        return {std::string(n, '-'), b, static_cast<int>(n) * sc.gap};
    }
    if (n == 0) {
        return {a, std::string(m, '-'), static_cast<int>(m) * sc.gap};
    }
    if (m <= 1 || n <= 1) {
        return needlemanWunschBase(a, b, sc);
    }

    // Divide: split A at midpoint
    size_t midA = m / 2;
    std::string aLeft = a.substr(0, midA);
    std::string aRight = a.substr(midA);

    // Reversed strings for backward scoring
    std::string aRightRev = aRight;
    std::reverse(aRightRev.begin(), aRightRev.end());
    std::string bRev = b;
    std::reverse(bRev.begin(), bRev.end());

    std::vector<int> scoreLeft = getScoreRow(aLeft, b, sc);
    std::vector<int> scoreRight = getScoreRow(aRightRev, bRev, sc);

    // Conquer: Find optimal split point in B
    int maxScore = -1e9;
    size_t splitB = 0;
    for (size_t j = 0; j <= n; ++j) {
        int total = scoreLeft[j] + scoreRight[n - j];
        if (total > maxScore) {
            maxScore = total;
            splitB = j;
        }
    }

    // Recurse on halves
    auto resLeft = hirschberg(aLeft, b.substr(0, splitB), sc);
    auto resRight = hirschberg(aRight, b.substr(splitB), sc);

    return {
        resLeft.alignedA + resRight.alignedA,
        resLeft.alignedB + resRight.alignedB,
        resLeft.score + resRight.score
    };
}

} // namespace bio

int main() {
    std::cout << "========================================\n";
    std::cout << "   HIRSCHBERG LINEAR-SPACE ALIGNMENT    \n";
    std::cout << " (Global DNA / Protein Sequence Match)  \n";
    std::cout << "========================================\n\n";

    bio::Scoring sc; // match = +2, mismatch = -1, gap = -2

    std::string seqA = "GCATGCG";
    std::string seqB = "GATTACA";

    std::cout << "Sequence A: " << seqA << " (Length " << seqA.length() << ")\n";
    std::cout << "Sequence B: " << seqB << " (Length " << seqB.length() << ")\n";
    std::cout << "Scoring   : Match = +" << sc.match << ", Mismatch = " << sc.mismatch
              << ", Gap = " << sc.gap << "\n\n";

    auto res = bio::hirschberg(seqA, seqB, sc);

    std::cout << "Optimal Global Alignment Result:\n";
    std::cout << "  Aligned A: " << res.alignedA << "\n";
    std::cout << "  Aligned B: " << res.alignedB << "\n";
    std::cout << "  Score    : " << res.score << "\n\n";

    // Verify alignment score matches
    int checkScore = 0;
    for (size_t i = 0; i < res.alignedA.length(); ++i) {
        char ca = res.alignedA[i];
        char cb = res.alignedB[i];
        if (ca == '-' || cb == '-') {
            checkScore += sc.gap;
        } else if (ca == cb) {
            checkScore += sc.match;
        } else {
            checkScore += sc.mismatch;
        }
    }
    std::cout << "Score Verification Check: " << (checkScore == res.score ? "EXACT MATCH" : "FAIL") << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
