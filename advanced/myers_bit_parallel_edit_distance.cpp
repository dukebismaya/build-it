/*
 * ============================================================
 *       Myers' Bit-Parallel O((M/64) N) Edit Distance
 * ============================================================
 *
 * Description:
 *   Gene Myers' 1999 algorithm ("A Fast Bit-Vector Algorithm for
 *   Approximate String Matching Based on Dynamic Programming")
 *   accelerates Levenshtein edit distance by encoding the vertical
 *   differences Delta_v[i] = C[i, j] - C[i-1, j] in {-1, 0, +1} of
 *   an entire 64-row column into two 64-bit bitmasks:
 *     - Pv (positive vertical delta: bit i is 1 iff C[i] - C[i-1] == +1)
 *     - Mv (negative vertical delta: bit i is 1 iff C[i] - C[i-1] == -1)
 *
 *   For each character T[j] of the text, the new column's Pv and Mv
 *   are computed in O(1) CPU instructions using precomputed pattern
 *   character equality bitmasks Peq[c]:
 *     Xv = Eq | Mv
 *     Xh = (((Eq & Pv) + Pv) ^ Pv) | Eq
 *     Ph = Mv | ~(Xh | Pv)
 *     Mh = Pv & Xh
 *     score += (Ph >> (M - 1)) & 1 - ((Mh >> (M - 1)) & 1)
 *     Ph = (Ph << 1) | 1;  Mh = Mh << 1
 *     Pv = Mh | ~(Xv | Ph); Mv = Ph & Xv
 *
 * Complexity:
 *   Time:  O(N) for pattern length M <= 64 (64x speedup over O(MN) DP!)
 *   Space: O(|Sigma|) for the 256-entry pattern mask table
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <random>
#include <chrono>
#include <algorithm>

namespace myers_bitparallel {

    // Exact global Levenshtein edit distance for pattern length M <= 64
    int edit_distance_bitparallel(const std::string& pattern, const std::string& text) {
        int m = static_cast<int>(pattern.size());
        if (m == 0) return static_cast<int>(text.size());

        std::array<uint64_t, 256> peq{};
        for (int i = 0; i < m; ++i)
            peq[static_cast<unsigned char>(pattern[i])] |= (1ULL << i);

        uint64_t pv = ~0ULL;
        uint64_t mv = 0ULL;
        uint64_t high_bit = 1ULL << (m - 1);
        int score = m;

        for (unsigned char c : text) {
            uint64_t eq = peq[c];
            uint64_t xv = eq | mv;
            uint64_t xh = (((eq & pv) + pv) ^ pv) | eq;

            uint64_t ph = mv | ~(xh | pv);
            uint64_t mh = pv & xh;

            if (ph & high_bit) ++score;
            if (mh & high_bit) --score;

            // Horizontal boundary at row 0 is always +1 for global edit distance
            ph = (ph << 1) | 1ULL;
            mh = (mh << 1);

            pv = mh | ~(xv | ph);
            mv = ph & xv;
        }
        return score;
    }

    int edit_distance_dp(const std::string& a, const std::string& b) {
        int m = static_cast<int>(a.size()), n = static_cast<int>(b.size());
        std::vector<int> dp(m + 1);
        for (int i = 0; i <= m; ++i) dp[i] = i;
        for (int j = 1; j <= n; ++j) {
            int prev_diag = dp[0];
            dp[0] = j;
            for (int i = 1; i <= m; ++i) {
                int temp = dp[i];
                if (a[i - 1] == b[j - 1])
                    dp[i] = prev_diag;
                else
                    dp[i] = 1 + std::min({prev_diag, dp[i], dp[i - 1]});
                prev_diag = temp;
            }
        }
        return dp[m];
    }

} // namespace myers_bitparallel

int main() {
    using namespace myers_bitparallel;
    std::cout << "============================================================\n";
    std::cout << "       Myers' Bit-Parallel O((M/64) N) Edit Distance\n";
    std::cout << "============================================================\n\n";

    std::pair<std::string, std::string> samples[] = {
        {"kitten", "sitting"},
        {"saturday", "sunday"},
        {"exponential", "polynomial"},
        {"GATTACA_GATTACA_GATTACA", "GCATGCU_GATTACA_GACTATA"}
    };
    for (const auto& [p, t] : samples) {
        std::cout << "d(\"" << p << "\", \"" << t << "\") = "
                  << edit_distance_bitparallel(p, t)
                  << "  [DP = " << edit_distance_dp(p, t) << "]\n";
    }

    std::mt19937 rng(2026);
    bool ok = true;
    for (int trial = 0; trial < 2000 && ok; ++trial) {
        int m = 1 + rng() % 64;
        int n = rng() % 120;
        std::string pat(m, ' '), txt(n, ' ');
        int alpha = 2 + rng() % 6;
        for (char& c : pat) c = static_cast<char>('A' + rng() % alpha);
        for (char& c : txt) c = static_cast<char>('A' + rng() % alpha);
        if (edit_distance_bitparallel(pat, txt) != edit_distance_dp(pat, txt))
            ok = false;
    }

    // Benchmark on a 64-bp DNA query against a 200,000-bp sequence
    std::string dna_q(64, 'A'), dna_t(200000, 'A');
    const char bases[] = "ACGT";
    for (char& c : dna_q) c = bases[rng() & 3];
    for (char& c : dna_t) c = bases[rng() & 3];

    auto t0 = std::chrono::steady_clock::now();
    int d_bp = edit_distance_bitparallel(dna_q, dna_t);
    auto t1 = std::chrono::steady_clock::now();
    int d_dp = edit_distance_dp(dna_q, dna_t);
    auto t2 = std::chrono::steady_clock::now();
    if (d_bp != d_dp) ok = false;

    std::cout << "\n64-char query vs 200,000-char text (distance = " << d_bp << "):\n";
    std::cout << "  Myers Bit-Parallel : " << std::chrono::duration<double, std::milli>(t1 - t0).count() << " ms\n";
    std::cout << "  Standard O(MN) DP  : " << std::chrono::duration<double, std::milli>(t2 - t1).count() << " ms\n";
    std::cout << "Random verification (2000 pairs, M <= 64): " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
