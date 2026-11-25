/*
 * ============================================================
 *       SMAWK Algorithm: Totally Monotone Matrix Row Minima
 * ============================================================
 *
 * Description:
 *   An N x M matrix A is totally monotone if for all i1 < i2 and
 *   j1 < j2, A[i1, j1] > A[i1, j2] => A[i2, j1] > A[i2, j2]
 *   (in particular, every Monge matrix satisfying
 *    A[i1, j1] + A[i2, j2] <= A[i1, j2] + A[i2, j1] is totally
 *   monotone).
 *
 *   The SMAWK algorithm (Shor, Moran, Aggarwal, Wilber, Klawe,
 *   1987) finds the column index of the minimum in EVERY row
 *   using only O(N + M) matrix lookups:
 *     1. REDUCE: Given r rows and c > r columns, eliminate at
 *        least c - r columns that can never be row minima in O(c)
 *        comparisons using a monotone stack.
 *     2. INTERPOLATE: Recursively solve on the even-indexed rows
 *        (r/2 rows, at most r columns), then fill in the odd rows
 *        by scanning between the neighboring even rows' optimal
 *        columns in O(r + c) total work.
 *
 *   Application: Min-plus (tropical) convolution c[k] = min_{i+j=k}
 *   (a[i] + b[j]) where b is convex, computed in O(N + M) time.
 *
 * Complexity:
 *   T(N, M) = O(N + M) matrix evaluations, O(N + M) space.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <functional>
#include <climits>
#include <random>
#include <algorithm>

namespace smawk {

    // Returns argmin column (from cols) for each row in rows.
    // Lookup f(row, col) is evaluated O(|rows| + |cols|) times.
    template <typename Lookup>
    std::vector<int> row_minima(const std::vector<int>& rows,
                                const std::vector<int>& cols,
                                const Lookup& f) {
        if (rows.empty()) return {};

        // Step 1: REDUCE columns to at most |rows|
        std::vector<int> red;
        red.reserve(rows.size());
        for (int c : cols) {
            while (!red.empty()) {
                int r = rows[red.size() - 1];
                if (f(r, red.back()) <= f(r, c)) break;
                red.pop_back();
            }
            if (red.size() < rows.size()) red.push_back(c);
        }

        // Step 2: Recurse on odd positions (indices 1, 3, 5, ...)
        std::vector<int> half_rows;
        for (size_t i = 1; i < rows.size(); i += 2) half_rows.push_back(rows[i]);
        auto half_ans = row_minima(half_rows, red, f);

        std::vector<int> ans(rows.size());
        for (size_t i = 0; i < half_ans.size(); ++i) ans[2 * i + 1] = half_ans[i];

        // Step 3: Interpolate even indices between neighboring bounds
        size_t ptr = 0;
        for (size_t i = 0; i < rows.size(); i += 2) {
            int r = rows[i];
            int stop_col = (i + 1 < rows.size()) ? ans[i + 1] : red.back();
            int best_col = red[ptr];
            auto best_val = f(r, best_col);
            while (red[ptr] != stop_col) {
                ++ptr;
                auto val = f(r, red[ptr]);
                if (val < best_val) { best_val = val; best_col = red[ptr]; }
            }
            ans[i] = best_col;
        }
        return ans;
    }

    // Min-plus convolution c[k] = min_{i + j = k} (a[i] + b[j]), where b is convex
    std::vector<long long> min_plus_conv_arbitrary_convex(const std::vector<long long>& a,
                                                          const std::vector<long long>& b) {
        int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
        int total = n + m - 1;
        const long long INF = LLONG_MAX / 4;
        auto lookup = [&](int r, int c) -> long long {
            int j = r - c;
            if (j < 0) return INF + (c - r);          // penalize out-of-range monotonically
            if (j >= m) return INF + (j - m + 1);
            return a[c] + b[j];
        };
        std::vector<int> rows(total), cols(n);
        for (int i = 0; i < total; ++i) rows[i] = i;
        for (int i = 0; i < n; ++i) cols[i] = i;
        auto best = row_minima(rows, cols, lookup);
        std::vector<long long> res(total);
        for (int k = 0; k < total; ++k) res[k] = lookup(k, best[k]);
        return res;
    }

} // namespace smawk

int main() {
    using namespace smawk;
    std::cout << "============================================================\n";
    std::cout << "      SMAWK Algorithm: Totally Monotone Matrix Row Minima\n";
    std::cout << "============================================================\n\n";

    std::vector<long long> a = {10, 3, 8, 1, 6, 4};
    std::vector<long long> b = {0, 1, 4, 9, 16, 25};   // convex: j^2
    auto c = min_plus_conv_arbitrary_convex(a, b);
    std::cout << "Min-plus convolution of a = [10,3,8,1,6,4] and convex b = [0,1,4,9,16,25]:\n  ";
    for (long long v : c) std::cout << v << " ";
    std::cout << "\n";

    std::mt19937 rng(42);
    bool ok = true;
    for (int trial = 0; trial < 100 && ok; ++trial) {
        int n = 10 + rng() % 80, m = 10 + rng() % 80;
        std::vector<long long> A(n), B(m);
        for (auto& x : A) x = static_cast<long long>(rng() % 1000) - 500;
        B[0] = rng() % 50;
        long long slope = static_cast<long long>(rng() % 40) - 20;
        for (int j = 1; j < m; ++j) {
            B[j] = B[j - 1] + slope;
            slope += rng() % 15;   // non-decreasing second differences => convex
        }
        auto got = min_plus_conv_arbitrary_convex(A, B);
        std::vector<long long> brute(n + m - 1, LLONG_MAX);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                brute[i + j] = std::min(brute[i + j], A[i] + B[j]);
        if (got != brute) ok = false;
    }
    std::cout << "\nRandom min-plus convolution vs O(NM) brute force (100 trials): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
