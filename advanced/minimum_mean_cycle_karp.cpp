/*
 * ============================================================
 *          Karp's Minimum Mean Weight Cycle Algorithm
 * ============================================================
 *
 * Description:
 *   Given a directed graph G = (V, E) with edge weights w(e)
 *   (possibly negative), the mean weight of a directed cycle C
 *   is (sum_{e in C} w(e)) / |C|.
 *
 *   Karp's Theorem (1978):
 *     Let d[k][v] be the minimum weight of a directed walk of
 *     EXACTLY k edges ending at vertex v (starting from any vertex,
 *     i.e., d[0][v] = 0 for all v). Then the minimum cycle mean is:
 *
 *       mu* = min_{v in V} max_{0 <= k <= N-1} (d[N][v] - d[k][v]) / (N - k)
 *
 *     provided G has at least one cycle (where vertices with
 *     d[N][v] = +infinity are skipped).
 *
 *   We also extract an explicit cycle achieving mu* by walking
 *   backwards along the predecessor table from the minimizing v
 *   for N steps and taking the unique simple cycle on that trail.
 *
 * Complexity:
 *   Time:  O(V * E)
 *   Space: O(V^2)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <random>
#include <iomanip>
#include <algorithm>

namespace karp_mmc {

    struct Edge { int u, v; long long w; };

    struct Result {
        bool has_cycle;
        long long num, den;          // exact fraction mu* = num / den (den > 0)
        double mean;
        std::vector<int> cycle_nodes;
    };

    Result minimum_mean_cycle(int n, const std::vector<Edge>& edges) {
        const long long INF = LLONG_MAX / 4;
        std::vector<std::vector<long long>> d(n + 1, std::vector<long long>(n, INF));
        std::vector<std::vector<int>> parent_v(n + 1, std::vector<int>(n, -1));
        for (int v = 0; v < n; ++v) d[0][v] = 0;

        for (int k = 1; k <= n; ++k) {
            for (const auto& e : edges) {
                if (d[k - 1][e.u] < INF && d[k - 1][e.u] + e.w < d[k][e.v]) {
                    d[k][e.v] = d[k - 1][e.u] + e.w;
                    parent_v[k][e.v] = e.u;
                }
            }
        }

        bool found = false;
        long long best_num = 0, best_den = 1;
        int best_v = -1;

        for (int v = 0; v < n; ++v) {
            if (d[n][v] >= INF) continue;
            bool v_has_k = false;
            long long max_num = 0, max_den = 1;
            for (int k = 0; k < n; ++k) {
                if (d[k][v] >= INF) continue;
                long long num = d[n][v] - d[k][v], den = n - k;
                if (!v_has_k || num * max_den > max_num * den) {
                    max_num = num;
                    max_den = den;
                    v_has_k = true;
                }
            }
            if (v_has_k && (!found || max_num * best_den < best_num * max_den)) {
                best_num = max_num;
                best_den = max_den;
                best_v = v;
                found = true;
            }
        }

        if (!found) return {false, 0, 1, 0.0, {}};

        // Extract cycle on the N-step predecessor trail from best_v
        std::vector<int> trail;
        std::vector<int> seen(n, -1);
        int cur = best_v;
        for (int step = n; step >= 0; --step) {
            trail.push_back(cur);
            if (step > 0) cur = parent_v[step][cur];
        }
        std::reverse(trail.begin(), trail.end());
        std::vector<int> cyc;
        for (int i = 0; i < static_cast<int>(trail.size()); ++i) {
            int u = trail[i];
            if (seen[u] != -1) {
                cyc.assign(trail.begin() + seen[u], trail.begin() + i);
                break;
            }
            seen[u] = i;
        }
        return {true, best_num, best_den, static_cast<double>(best_num) / best_den, cyc};
    }

    // Independent verification via binary search + Bellman-Ford negative cycle detection
    double bellman_ford_binary_search(int n, const std::vector<Edge>& edges) {
        double lo = -1000.0, hi = 1000.0;
        for (int it = 0; it < 60; ++it) {
            double mid = 0.5 * (lo + hi);
            std::vector<double> dist(n, 0.0);
            bool neg = false;
            for (int step = 1; step <= n; ++step) {
                bool upd = false;
                for (const auto& e : edges) {
                    if (dist[e.u] + (e.w - mid) < dist[e.v] - 1e-12) {
                        dist[e.v] = dist[e.u] + (e.w - mid);
                        upd = true;
                        if (step == n) neg = true;
                    }
                }
                if (!upd) break;
            }
            if (neg) hi = mid; else lo = mid;
        }
        return 0.5 * (lo + hi);
    }

} // namespace karp_mmc

int main() {
    using namespace karp_mmc;
    std::cout << "============================================================\n";
    std::cout << "          Karp's Minimum Mean Weight Cycle Algorithm\n";
    std::cout << "============================================================\n\n";

    std::vector<Edge> demo = {
        {0, 1, 1}, {1, 2, 3}, {2, 0, -1},   // cycle 0->1->2->0: sum 3, len 3 => mean 1.0
        {1, 3, 2}, {3, 2, -4},              // cycle 1->3->2->0->1: sum -2, len 4 => mean -0.5
        {2, 3, 5}
    };
    auto r = minimum_mean_cycle(4, demo);
    std::cout << "Demo graph minimum cycle mean = " << r.num << "/" << r.den
              << " (" << std::fixed << std::setprecision(4) << r.mean << ")\n";

    std::mt19937 rng(707);
    bool ok = true;
    for (int trial = 0; trial < 100 && ok; ++trial) {
        int n = 6 + rng() % 18;
        std::vector<Edge> edges;
        // Guarantee at least one cycle
        for (int i = 0; i < n; ++i)
            edges.push_back({i, (i + 1) % n, static_cast<long long>(rng() % 101) - 50});
        int extra = rng() % (2 * n);
        for (int i = 0; i < extra; ++i) {
            int u = rng() % n, v = rng() % n;
            if (u != v) edges.push_back({u, v, static_cast<long long>(rng() % 101) - 50});
        }
        auto karp = minimum_mean_cycle(n, edges);
        double bf = bellman_ford_binary_search(n, edges);
        if (!karp.has_cycle || std::fabs(karp.mean - bf) > 1e-6) ok = false;
    }

    std::cout << "\nRandom directed graphs: Karp exact formula vs Bellman-Ford binary search: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
