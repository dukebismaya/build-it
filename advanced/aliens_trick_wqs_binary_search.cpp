/**
 * ============================================================================
 * Alien's Trick (WQS Binary Search / Lagrangian DP Optimization)
 * ============================================================================
 * 
 * Description:
 * Alien's Trick (popularized by IOI 2016 "Aliens", also known as WQS Binary
 * Search or Lagrangian Relaxation in operations research) is a technique that
 * optimizes dynamic programming problems with an exact cardinality constraint:
 *   "Find optimal value subject to using EXACTLY (or at most) K operations."
 * 
 * Mathematical Formulation:
 * Let f(k) denote the optimal DP objective value when choosing exactly k items.
 * If the function f(k) is strictly concave (or convex, depending on max/min),
 * the marginal difference:
 *   f(k) - f(k - 1) >= f(k + 1) - f(k)  (diminishing returns)
 * 
 * We introduce a Lagrange multiplier (or penalty / bonus) lambda for each
 * operation performed. Consider the unconstrained problem:
 *   g(lambda) = max_k { f(k) - lambda * k }
 * 
 * Geometrically:
 * - The line y = lambda * x + g(lambda) is a supporting tangent line of slope lambda
 *   to the upper convex hull of (k, f(k)).
 * - For a chosen penalty lambda, solving the unconstrained DP takes O(N) time
 *   and naturally finds the optimal number of choices k*(lambda).
 * - Since the slopes of chords are monotonically non-increasing as k increases,
 *   k*(lambda) is monotonically non-increasing with respect to lambda.
 * - We can binary search on lambda to find a slope where k*(lambda) = K.
 * - Once lambda is found, the original constrained answer is:
 *     f(K) = g(lambda) + K * lambda.
 * 
 * Time Complexity:
 * - Unconstrained DP evaluation: O(N)
 * - Binary search over lambda: O(log(ValueRange))
 * - Total Runtime: O(N * log(ValueRange))
 * - Space Complexity: O(N) or O(1) auxiliary state
 * 
 * Concrete Problem Solved:
 * "Given an array of N integers (positive and negative), select exactly K
 * non-overlapping, non-empty contiguous subarrays to maximize their total sum."
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <limits>

namespace aliens_trick {

constexpr int64_t INF = 1e16;

struct DPState {
    int64_t profit; // Maximum penalized profit: sum - lambda * count
    int32_t count;  // Number of subarrays chosen
};

// Returns maximum penalized sum and count of subarrays for a given penalty lambda.
// If multiple choices have the same penalized profit, we break ties by MAXIMIZING
// the number of subarrays chosen (or minimizing, consistently).
DPState solveUnconstrainedDP(const std::vector<int64_t>& arr, int64_t lambda) {
    int n = static_cast<int>(arr.size());

    // dp_out: best state at step i where arr[i] is NOT part of an ongoing subarray
    // dp_in:  best state at step i where arr[i] IS part of an ongoing subarray
    DPState dp_out{0, 0};
    DPState dp_in{-INF, 0};

    for (int i = 0; i < n; ++i) {
        // Option 1 for continuing in:
        // Either extend the previous ongoing subarray (no new penalty): dp_in.profit + arr[i]
        // Or start a NEW subarray at i (costs penalty lambda): dp_out.profit + arr[i] - lambda
        DPState extend{dp_in.profit + arr[i], dp_in.count};
        DPState start_new{dp_out.profit + arr[i] - lambda, dp_out.count + 1};

        DPState next_in;
        if (extend.profit > start_new.profit) {
            next_in = extend;
        } else if (extend.profit < start_new.profit) {
            next_in = start_new;
        } else {
            // Tie-break: prefer larger count to establish upper bound of flat slope
            next_in = (extend.count >= start_new.count) ? extend : start_new;
        }

        // Option 2 for being out:
        // Either stay out (dp_out) or close an ongoing subarray (next_in)
        DPState next_out = dp_out;
        if (next_in.profit > next_out.profit) {
            next_out = next_in;
        } else if (next_in.profit == next_out.profit) {
            if (next_in.count > next_out.count) {
                next_out = next_in;
            }
        }

        dp_in = next_in;
        dp_out = next_out;
    }

    return dp_out;
}

/**
 * Solves the constrained K-subarray problem using Alien's trick.
 * Binary searches the integer penalty lambda.
 */
int64_t maxKSubarraysSum(const std::vector<int64_t>& arr, int k) {
    int n = static_cast<int>(arr.size());
    if (k <= 0 || k > n) return 0;

    // Determine binary search bounds for lambda.
    // The maximum possible contribution of an element is bounded by max |arr[i]|.
    int64_t max_elem = 0;
    for (int64_t v : arr) {
        max_elem = std::max(max_elem, std::abs(v));
    }

    int64_t low = -2 * max_elem * n;
    int64_t high = 2 * max_elem * n;
    int64_t best_lambda = 0;

    // Binary search for lambda
    while (low <= high) {
        int64_t mid = low + (high - low) / 2;
        DPState res = solveUnconstrainedDP(arr, mid);

        if (res.count >= k) {
            // If we selected >= k subarrays, the penalty is low enough.
            // Record lambda and try increasing penalty (move right) to reduce count.
            best_lambda = mid;
            low = mid + 1;
        } else {
            // Too few subarrays; penalty mid is too strict. Decrease penalty.
            high = mid - 1;
        }
    }

    // Recover exact f(K):
    // Even if res.count > K due to collinearities (flat tangent segments),
    // any point on the tangent line satisfies:
    // f(K) = g(best_lambda) + K * best_lambda
    DPState optimal = solveUnconstrainedDP(arr, best_lambda);
    return optimal.profit + static_cast<int64_t>(k) * best_lambda;
}

/**
 * Standard O(K * N) dynamic programming approach for validation.
 * dp[i][j]: maximum sum using j subarrays from prefix of length i.
 */
int64_t standardDPValidation(const std::vector<int64_t>& arr, int k) {
    int n = static_cast<int>(arr.size());
    if (k <= 0 || k > n) return 0;

    // dp[j][0]: prefix max with j subarrays, not inside subarray
    // dp[j][1]: prefix max with j subarrays, inside subarray
    std::vector<std::vector<int64_t>> dp(k + 1, std::vector<int64_t>(2, -INF));
    dp[0][0] = 0;

    for (int i = 0; i < n; ++i) {
        std::vector<std::vector<int64_t>> next_dp = dp;
        for (int j = 0; j <= k; ++j) {
            if (j >= 1) {
                // Start a new subarray
                if (dp[j - 1][0] != -INF) {
                    next_dp[j][1] = std::max(next_dp[j][1], dp[j - 1][0] + arr[i]);
                }
                // Continue active subarray
                if (dp[j][1] != -INF) {
                    next_dp[j][1] = std::max(next_dp[j][1], dp[j][1] + arr[i]);
                }
            }
            // End active subarray or remain idle
            next_dp[j][0] = std::max(next_dp[j][0], next_dp[j][1]);
        }
        dp = next_dp;
    }

    return dp[k][0];
}

} // namespace aliens_trick

int main() {
    std::cout << "========================================================\n";
    std::cout << " Alien's Trick (WQS Binary Search / Lagrangian DP)\n";
    std::cout << "========================================================\n\n";

    std::vector<int64_t> sample = {5, -3, 6, 2, -10, 8, -2, 7, -1, 4};
    int n = static_cast<int>(sample.size());

    std::cout << "Input Array (" << n << " elements):\n [";
    for (int i = 0; i < n; ++i) {
        std::cout << sample[i] << (i + 1 < n ? ", " : "]\n\n");
    }

    std::cout << "Evaluating maximum subarray sum for each K in [1, 5]:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(6) << "K"
              << std::setw(18) << "Aliens Trick"
              << std::setw(18) << "Standard DP"
              << std::setw(14) << "Match?\n";
    std::cout << "--------------------------------------------------------\n";

    for (int k = 1; k <= 5; ++k) {
        int64_t aliens_ans = aliens_trick::maxKSubarraysSum(sample, k);
        int64_t brute_ans = aliens_trick::standardDPValidation(sample, k);

        std::cout << std::setw(6) << k
                  << std::setw(18) << aliens_ans
                  << std::setw(18) << brute_ans
                  << std::setw(14) << (aliens_ans == brute_ans ? "YES" : "MISMATCH")
                  << "\n";
    }
    std::cout << "--------------------------------------------------------\n\n";

    // Stress test comparison with a larger random-like synthetic sequence
    std::cout << "Stress Test Comparison on Length 20 Sequence:\n";
    std::vector<int64_t> stress_seq = {
        12, -7, 4, 18, -25, 9, 14, -3, 22, -15,
        30, -5, -8, 17, 6, -11, 19, -4, 13, 2
    };

    bool all_passed = true;
    for (int k = 1; k <= 6; ++k) {
        int64_t a = aliens_trick::maxKSubarraysSum(stress_seq, k);
        int64_t b = aliens_trick::standardDPValidation(stress_seq, k);
        if (a != b) {
            all_passed = false;
            std::cout << " Discrepancy at K = " << k << ": Aliens=" << a << " Brute=" << b << "\n";
        }
    }

    if (all_passed) {
        std::cout << " -> All K tests in [1, 6] matched identically between O(N log C) and O(K*N)!\n";
    }

    return 0;
}
