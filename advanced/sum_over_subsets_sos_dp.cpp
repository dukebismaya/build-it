/**
 * ============================================================================
 * Sum Over Subsets (SOS DP / Yates's Dynamic Programming Algorithm)
 * ============================================================================
 * 
 * Description:
 * Given an array A of size 2^N, the Sum Over Subsets (SOS) problem asks us
 * to compute for every bitmask in [0, 2^N - 1]:
 *   F[mask] = sum_{sub is a submask of mask} A[sub]
 * 
 * Comparison of Approaches:
 * 1. Naive Submask Iteration:
 *    Iterating over all submasks for each mask:
 *      for (int mask = 0; mask < (1<<N); ++mask)
 *        for (int sub = mask; sub > 0; sub = (sub - 1) & mask)
 *    Time Complexity: sum_{k=0}^N C(N, k) * 2^k = 3^N.
 *    For N = 20, 3^20 ~ 3.48 * 10^9 operations (Too slow / TLE).
 * 
 * 2. SOS DP (Yates's Algorithm):
 *    State definition:
 *      dp[i][mask] = sum of A[sub] where sub and mask can differ ONLY in
 *                    the first i bits (bits 0 .. i - 1).
 *    Transitions:
 *      - If the i-th bit of mask is 0:
 *          dp[i][mask] = dp[i - 1][mask]
 *      - If the i-th bit of mask is 1:
 *          dp[i][mask] = dp[i - 1][mask] + dp[i - 1][mask ^ (1 << (i - 1))]
 * 
 * Memory Optimization:
 * The outer dimension i can be flattened into a single 1D array:
 *   for (int i = 0; i < N; ++i)
 *     for (int mask = 0; mask < (1 << N); ++mask)
 *       if (mask & (1 << i))
 *         F[mask] += F[mask ^ (1 << i)];
 * 
 * Dual Problem (Superset Sum DP):
 *   G[mask] = sum_{mask is a submask of super} A[super]
 *   Calculated by traversing in reverse: G[mask] += G[mask | (1 << i)].
 * 
 * Time Complexity:
 * - SOS DP Runtime: O(N * 2^N)
 * - Space Complexity: O(2^N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <bitset>

namespace sos_dp {

/**
 * Computes submask sum F[mask] = sum_{sub subseteq mask} A[sub] in O(N * 2^N).
 */
std::vector<int64_t> computeSubmaskSums(std::vector<int64_t> a, int n) {
    int total_masks = 1 << n;

    for (int i = 0; i < n; ++i) {
        for (int mask = 0; mask < total_masks; ++mask) {
            if (mask & (1 << i)) {
                a[mask] += a[mask ^ (1 << i)];
            }
        }
    }

    return a;
}

/**
 * Computes superset sum G[mask] = sum_{mask subseteq super} A[super] in O(N * 2^N).
 */
std::vector<int64_t> computeSupersetSums(std::vector<int64_t> a, int n) {
    int total_masks = 1 << n;

    for (int i = 0; i < n; ++i) {
        for (int mask = total_masks - 1; mask >= 0; --mask) {
            if (!(mask & (1 << i))) {
                a[mask] += a[mask | (1 << i)];
            }
        }
    }

    return a;
}

/**
 * Brute force submask summation in O(3^N) for verification.
 */
std::vector<int64_t> bruteSubmaskSums(const std::vector<int64_t>& a, int n) {
    int total_masks = 1 << n;
    std::vector<int64_t> f(total_masks, 0);

    for (int mask = 0; mask < total_masks; ++mask) {
        for (int sub = mask; ; sub = (sub - 1) & mask) {
            f[mask] += a[sub];
            if (sub == 0) break;
        }
    }

    return f;
}

} // namespace sos_dp

int main() {
    std::cout << "========================================================\n";
    std::cout << " Sum Over Subsets (SOS DP / Yates's Algorithm)\n";
    std::cout << "========================================================\n\n";

    constexpr int N = 4; // 16 bitmasks
    int total = 1 << N;

    std::vector<int64_t> a = {
        3, 1, 4, 1, 5, 9, 2, 6,
        5, 3, 5, 8, 9, 7, 9, 3
    };

    std::cout << "Input Array A (2^" << N << " = " << total << " elements):\n";
    for (int mask = 0; mask < total; ++mask) {
        std::bitset<N> bits(mask);
        std::cout << "  mask " << std::setw(2) << mask
                  << " (" << bits.to_string() << "): " << a[mask] << "\n";
    }

    auto sos_submask = sos_dp::computeSubmaskSums(a, N);
    auto brute_submask = sos_dp::bruteSubmaskSums(a, N);
    auto sos_superset = sos_dp::computeSupersetSums(a, N);

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Results & Verification against O(3^N) Brute Force:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(6) << "Mask"
              << std::setw(8) << "Bits"
              << std::setw(15) << "Submask Sum"
              << std::setw(15) << "Brute Sum"
              << std::setw(15) << "Superset Sum\n";
    std::cout << "--------------------------------------------------------\n";

    bool match = true;
    for (int mask = 0; mask < total; ++mask) {
        std::bitset<N> bits(mask);
        std::cout << std::setw(6) << mask
                  << std::setw(8) << bits.to_string()
                  << std::setw(15) << sos_submask[mask]
                  << std::setw(15) << brute_submask[mask]
                  << std::setw(15) << sos_superset[mask]
                  << "\n";
        if (sos_submask[mask] != brute_submask[mask]) {
            match = false;
        }
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Submask DP Verification: "
              << (match ? "PASSED (100% Exact Match in O(N * 2^N))" : "FAILED")
              << "\n";

    return 0;
}
