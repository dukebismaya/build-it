/**
 * @file karp_rabin_2d_pattern_matching.cpp
 * @brief Searches for a 2D pattern (R x C subgrid) inside a 2D text (M x N matrix) via 2D Rabin-Karp rolling hash.
 *
 * Problem:
 * Given an M x N character matrix (Text) and an R x C character matrix (Pattern),
 * find all top-left coordinates (r, c) where the pattern appears as a subgrid.
 *
 * Algorithm (2D Polynomial Rolling Hash):
 * 1. Compute the pattern hash using two polynomial bases (B1 for rows, B2 for columns) modulo P.
 * 2. Hash each row horizontally:
 *    For every row of text, compute the 1D rolling hash of each window of length C.
 * 3. Hash vertically across rows:
 *    For each column of 1D row hashes, compute the rolling hash of length R.
 * 4. The resulting 2D hash matrix directly gives the hash of every R x C subgrid of the text.
 * 5. Compare with pattern hash; verify matching characters to prevent collisions.
 *
 * Time Complexity: O(M * N) average time
 * Space Complexity: O(M * N)
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

struct MatchPosition {
    int row;
    int col;
};

class RabinKarp2D {
private:
    static constexpr long long B1 = 313;         // Base for vertical/rows
    static constexpr long long B2 = 317;         // Base for horizontal/cols
    static constexpr long long MOD = 1000000007; // Prime modulus

public:
    static std::vector<MatchPosition> search(
        const std::vector<std::string>& text,
        const std::vector<std::string>& pattern) {
        std::vector<MatchPosition> matches;

        int M = static_cast<int>(text.size());
        if (M == 0) return matches;
        int N = static_cast<int>(text[0].size());

        int R = static_cast<int>(pattern.size());
        if (R == 0) return matches;
        int C = static_cast<int>(pattern[0].size());

        if (M < R || N < C) return matches;

        // Compute B2^(C-1) and B1^(R-1) modulo MOD
        long long powB2 = 1;
        for (int j = 0; j < C - 1; ++j) {
            powB2 = (powB2 * B2) % MOD;
        }

        long long powB1 = 1;
        for (int i = 0; i < R - 1; ++i) {
            powB1 = (powB1 * B1) % MOD;
        }

        // 1. Compute Pattern 2D Hash
        long long patternHash = 0;
        for (int i = 0; i < R; ++i) {
            long long rowH = 0;
            for (int j = 0; j < C; ++j) {
                rowH = (rowH * B2 + static_cast<unsigned char>(pattern[i][j])) % MOD;
            }
            patternHash = (patternHash * B1 + rowH) % MOD;
        }

        // 2. Horizontally hash each row of text for windows of length C
        // horizHash[i][j] = hash of text[i][j .. j+C-1]
        std::vector<std::vector<long long>> horizHash(M, std::vector<long long>(N - C + 1, 0));

        for (int i = 0; i < M; ++i) {
            long long curH = 0;
            for (int j = 0; j < C; ++j) {
                curH = (curH * B2 + static_cast<unsigned char>(text[i][j])) % MOD;
            }
            horizHash[i][0] = curH;

            for (int j = 1; j <= N - C; ++j) {
                curH = (curH - static_cast<unsigned char>(text[i][j - 1]) * powB2) % MOD;
                if (curH < 0) curH += MOD;
                curH = (curH * B2 + static_cast<unsigned char>(text[i][j + C - 1])) % MOD;
                horizHash[i][j] = curH;
            }
        }

        // 3. Vertically hash each column of horizHash for windows of length R
        for (int j = 0; j <= N - C; ++j) {
            long long cur2DH = 0;
            for (int i = 0; i < R; ++i) {
                cur2DH = (cur2DH * B1 + horizHash[i][j]) % MOD;
            }

            // Check position (0, j)
            if (cur2DH == patternHash) {
                // Verify exact characters
                bool ok = true;
                for (int r = 0; r < R && ok; ++r) {
                    for (int c = 0; c < C && ok; ++c) {
                        if (text[r][j + c] != pattern[r][c]) ok = false;
                    }
                }
                if (ok) matches.push_back({0, j});
            }

            for (int i = 1; i <= M - R; ++i) {
                cur2DH = (cur2DH - horizHash[i - 1][j] * powB1) % MOD;
                if (cur2DH < 0) cur2DH += MOD;
                cur2DH = (cur2DH * B1 + horizHash[i + R - 1][j]) % MOD;

                if (cur2DH == patternHash) {
                    bool ok = true;
                    for (int r = 0; r < R && ok; ++r) {
                        for (int c = 0; c < C && ok; ++c) {
                            if (text[i + r][j + c] != pattern[r][c]) ok = false;
                        }
                    }
                    if (ok) matches.push_back({i, j});
                }
            }
        }

        return matches;
    }
};

int main() {
    std::cout << "=== 2D Rabin-Karp Pattern Matching (O(M * N)) ===\n\n";

    // 5x7 Text Grid
    std::vector<std::string> text = {
        "ABCDCDE",
        "EFABCDC",
        "CDEABCD",
        "ABCDCDE",
        "EFABCDC"
    };

    // 2x3 Pattern Grid
    std::vector<std::string> pattern = {
        "ABC",
        "CDE"
    };

    std::cout << "Text Grid (5 x 7):\n";
    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
        std::cout << "  [" << i << "] " << text[i] << '\n';
    }

    std::cout << "\nPattern Grid (2 x 3):\n";
    for (int i = 0; i < static_cast<int>(pattern.size()); ++i) {
        std::cout << "  [" << i << "] " << pattern[i] << '\n';
    }

    auto matches = RabinKarp2D::search(text, pattern);

    std::cout << "\nSearch Results:\n";
    std::cout << "  Found " << matches.size() << " exact occurrence(s):\n";
    for (const auto& pos : matches) {
        std::cout << "  -> Top-left coordinate: (Row " << pos.row << ", Col " << pos.col << ")\n";
    }

    return 0;
}
