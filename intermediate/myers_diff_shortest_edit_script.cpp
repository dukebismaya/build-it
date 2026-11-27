/*
 * ============================================================
 *       Myers' O((N+M)D) Shortest Edit Script (git diff)
 * ============================================================
 *
 * Description:
 *   Eugene Myers' 1986 algorithm ("An O(ND) Difference Algorithm
 *   and Its Variations") is the default diff engine in `git diff`
 *   and GNU `diff`. Given sequences A (length N) and B (length M),
 *   it searches the edit graph level by level in the number of
 *   edits D = 0, 1, 2, ... (insertions + deletions, no substitutions):
 *
 *     - A diagonal k is defined by k = x - y.
 *     - A horizontal move (x + 1, y) deletes A[x] (shifts to k + 1).
 *     - A vertical move (x, y + 1) inserts B[y] (shifts to k - 1).
 *     - A diagonal "snake" (x + 1, y + 1) while A[x] == B[y] costs
 *       0 edits and stays on diagonal k.
 *     - Let V[d][k] be the furthest x-coordinate reached on diagonal
 *       k using at most d edits:
 *         x = max(V[d-1][k-1], V[d-1][k+1] + 1),  y = x - k
 *         extend snake while x < N and y < M and A[x] == B[y].
 *
 *   Because D is typically very small on real source files, O((N+M)D)
 *   is vastly faster than O(N*M) dynamic programming.
 *
 * Complexity:
 *   Time:  O((N + M) * D) where D is the minimum edit distance
 *   Space: O(D^2) to store V snapshots for edit-script backtracking.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

namespace myers_diff {

    enum class OpType { KEEP, INSERT, DELETE };

    template <typename T>
    struct EditOp {
        OpType type;
        T value;
    };

    template <typename Seq, typename T = typename Seq::value_type>
    std::vector<EditOp<T>> diff(const Seq& A, const Seq& B) {
        int n = static_cast<int>(A.size());
        int m = static_cast<int>(B.size());
        int max_d = n + m;
        int offset = max_d + 1;

        std::vector<int> v(2 * max_d + 3, 0);
        std::vector<std::vector<int>> trace;

        int final_d = 0;
        for (int d = 0; d <= max_d; ++d) {
            trace.push_back(v);
            for (int k = -d; k <= d; k += 2) {
                int x;
                if (k == -d || (k != d && v[offset + k - 1] < v[offset + k + 1]))
                    x = v[offset + k + 1];       // down from k+1 (INSERT B)
                else
                    x = v[offset + k - 1] + 1;   // right from k-1 (DELETE A)
                int y = x - k;
                while (x < n && y < m && A[x] == B[y]) { ++x; ++y; }
                v[offset + k] = x;
                if (x >= n && y >= m) {
                    final_d = d;
                    goto found;
                }
            }
        }
    found:
        std::vector<EditOp<T>> script;
        int x = n, y = m;
        for (int d = final_d; d > 0; --d) {
            const auto& vd = trace[d];
            int k = x - y;
            int prev_k;
            if (k == -d || (k != d && vd[offset + k - 1] < vd[offset + k + 1]))
                prev_k = k + 1;
            else
                prev_k = k - 1;

            int prev_x = vd[offset + prev_k];
            int prev_y = prev_x - prev_k;

            while (x > prev_x && y > prev_y) {
                --x; --y;
                script.push_back({OpType::KEEP, A[x]});
            }
            if (prev_k == k + 1) {
                --y;
                script.push_back({OpType::INSERT, B[y]});
            } else {
                --x;
                script.push_back({OpType::DELETE, A[x]});
            }
        }
        while (x > 0 && y > 0) {
            --x; --y;
            script.push_back({OpType::KEEP, A[x]});
        }
        std::reverse(script.begin(), script.end());
        return script;
    }

    int dp_lcs_length(const std::string& a, const std::string& b) {
        int n = static_cast<int>(a.size()), m = static_cast<int>(b.size());
        std::vector<int> dp(m + 1, 0), nxt(m + 1, 0);
        for (int i = 1; i <= n; ++i) {
            for (int j = 1; j <= m; ++j)
                nxt[j] = (a[i - 1] == b[j - 1]) ? dp[j - 1] + 1 : std::max(dp[j], nxt[j - 1]);
            dp.swap(nxt);
        }
        return dp[m];
    }

} // namespace myers_diff

int main() {
    using namespace myers_diff;
    std::cout << "============================================================\n";
    std::cout << "       Myers' O((N+M)D) Shortest Edit Script (git diff)\n";
    std::cout << "============================================================\n\n";

    std::vector<std::string> old_file = {
        "#include <iostream>",
        "int add(int a, int b) {",
        "    return a + b;",
        "}",
        "int main() {",
        "    std::cout << add(2, 3) << std::endl;",
        "    return 0;",
        "}"
    };
    std::vector<std::string> new_file = {
        "#include <iostream>",
        "constexpr int add(int a, int b) noexcept {",
        "    return a + b;",
        "}",
        "int main() {",
        "    std::cout << add(2, 3) << '\\n';",
        "    return 0;",
        "}"
    };

    auto line_diff = diff(old_file, new_file);
    std::cout << "Unified line diff example:\n";
    for (const auto& op : line_diff) {
        char prefix = (op.type == OpType::KEEP) ? ' ' : (op.type == OpType::INSERT) ? '+' : '-';
        std::cout << "  " << prefix << " " << op.value << "\n";
    }

    std::mt19937 rng(2026);
    bool ok = true;
    for (int trial = 0; trial < 300 && ok; ++trial) {
        int n = rng() % 40, m = rng() % 40;
        std::string a(n, ' '), b(m, ' ');
        for (char& c : a) c = static_cast<char>('A' + rng() % 4);
        for (char& c : b) c = static_cast<char>('A' + rng() % 4);

        auto ses = diff(a, b);
        std::string rec_a, rec_b;
        int keeps = 0, edits = 0;
        for (const auto& op : ses) {
            if (op.type == OpType::KEEP)        { rec_a += op.value; rec_b += op.value; ++keeps; }
            else if (op.type == OpType::DELETE) { rec_a += op.value; ++edits; }
            else                                { rec_b += op.value; ++edits; }
        }
        int lcs = dp_lcs_length(a, b);
        if (rec_a != a || rec_b != b || keeps != lcs || edits != n + m - 2 * lcs) ok = false;
    }

    std::cout << "\nRandom verification vs O(NM) LCS DP (300 string pairs): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
