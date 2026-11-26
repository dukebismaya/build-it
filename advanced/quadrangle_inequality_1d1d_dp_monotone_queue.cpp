/*
 * ============================================================
 *    1D/1D Quadrangle-Inequality DP via Monotone Decision Queue
 * ============================================================
 *
 * Description:
 *   Consider the 1D/1D dynamic programming recurrence:
 *     dp[i] = min_{0 <= j < i} (dp[j] + w(j, i)),   i = 1..N
 *   where the cost function w(j, i) satisfies the Quadrangle
 *   Inequality (Monge / convexity condition):
 *     w(a, c) + w(b, d) <= w(a, d) + w(b, c)   for a <= b <= c <= d.
 *
 *   Then the optimal decision opt[i] is monotonically non-decreasing:
 *     opt[1] <= opt[2] <= ... <= opt[N].
 *   Moreover, for any two candidate transitions j1 < j2, the set
 *   of indices i where j2 is at least as good as j1 forms a
 *   contiguous suffix [cross(j1, j2), N].
 *
 *   Algorithm (Galil & Giancarlo / Wilber Monotone Decision Deque):
 *     Maintain a deque of active intervals (j, l, r) meaning candidate
 *     j is currently optimal for all i in [l, r]:
 *     1. At step i, advance the front of the deque to find opt[i]
 *        and compute dp[i].
 *     2. Insert i as a new candidate at the back: while i beats
 *        the back candidate j_back even at j_back's left endpoint
 *        l_back, pop j_back. Otherwise binary-search the exact
 *        crossover point in [l_back, r_back].
 *
 *   Demo application: Optimal Poem Line Justification with irregular
 *   power penalty |line_len - L|^p (p >= 1, e.g., p = 3).
 *
 * Complexity:
 *   Time:  O(N log N) cost evaluations (vs O(N^2) naive)
 *   Space: O(N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <random>
#include <algorithm>

namespace qi_1d1d {

    // Solves dp[i] = min_{0 <= j < i} (dp[j] + cost(j, i)) for i = 1..n
    template <typename CostFn>
    std::pair<std::vector<double>, std::vector<int>> solve_1d1d(int n, const CostFn& cost) {
        struct Segment { int j, l, r; };
        std::vector<double> dp(n + 1, 0.0);
        std::vector<int> opt(n + 1, 0);
        std::deque<Segment> dq;
        dq.push_back({0, 1, n});

        auto val = [&](int j, int i) { return dp[j] + cost(j, i); };

        for (int i = 1; i <= n; ++i) {
            while (!dq.empty() && dq.front().r < i) dq.pop_front();
            dq.front().l = i;
            int best_j = dq.front().j;
            dp[i] = val(best_j, i);
            opt[i] = best_j;

            if (i == n) break;
            if (dq.front().r == i) dq.pop_front();
            else dq.front().l = i + 1;

            // Insert candidate i for future positions [i+1 .. n]
            int pos = n + 1;
            while (!dq.empty()) {
                auto [j, l, r] = dq.back();
                if (val(i, l) <= val(j, l)) {
                    pos = l;
                    dq.pop_back();
                } else {
                    int lo = l, hi = r;
                    while (lo <= hi) {
                        int mid = (lo + hi) / 2;
                        if (val(i, mid) <= val(j, mid)) { pos = mid; hi = mid - 1; }
                        else lo = mid + 1;
                    }
                    dq.back().r = pos - 1;
                    break;
                }
            }
            if (pos <= n) dq.push_back({i, pos, n});
        }
        return {dp, opt};
    }

} // namespace qi_1d1d

int main() {
    using namespace qi_1d1d;
    std::cout << "============================================================\n";
    std::cout << "   1D/1D Quadrangle-Inequality DP via Monotone Decision Queue\n";
    std::cout << "============================================================\n\n";

    std::mt19937 rng(2026);
    bool ok = true;
    for (int trial = 0; trial < 60 && ok; ++trial) {
        int n = 40 + rng() % 160;
        double target_L = 25.0 + (rng() % 30);
        std::vector<double> pref(n + 1, 0.0);
        for (int i = 1; i <= n; ++i)
            pref[i] = pref[i - 1] + (2 + rng() % 12);

        // Convex penalty |(pref[i] - pref[j] + (i - j - 1)) - L|^3 + sqrt(i - j)
        auto cost = [&](int j, int i) {
            double len = (pref[i] - pref[j]) + (i - j - 1);
            double d = std::fabs(len - target_L);
            return d * d * d;
        };

        auto [dp_fast, opt_fast] = solve_1d1d(n, cost);

        std::vector<double> dp_brute(n + 1, 0.0);
        for (int i = 1; i <= n; ++i) {
            double best = 1e300;
            for (int j = 0; j < i; ++j)
                best = std::min(best, dp_brute[j] + cost(j, i));
            dp_brute[i] = best;
            if (std::fabs(dp_fast[i] - dp_brute[i]) > 1e-6 * std::max(1.0, dp_brute[i])) ok = false;
            if (i > 1 && opt_fast[i] < opt_fast[i - 1]) ok = false; // decision monotonicity
        }
    }

    std::cout << "Random poem justification instances (60 trials, N up to 200) vs O(N^2) DP: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
