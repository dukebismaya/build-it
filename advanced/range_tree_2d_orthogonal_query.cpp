/*
 * ============================================================
 *        2D Range Tree (Orthogonal Range Counting/Reporting)
 * ============================================================
 *
 * Description:
 *   A static 2D range tree answers axis-aligned rectangle
 *   queries [x1, x2] x [y1, y2] over a fixed point set.
 *
 *   Structure: a balanced tree over points sorted by x. Every
 *   node stores the y-coordinates of all points in its x-range,
 *   sorted (a merge-sort tree). A query decomposes [x1, x2] into
 *   O(log N) canonical nodes, then binary-searches [y1, y2] in
 *   each node's sorted y-list.
 *
 * Complexity:
 *   Build:  O(N log N) time and space
 *   Count:  O(log^2 N)
 *   Report: O(log^2 N + K) for K reported points
 *   (Fractional cascading would reduce queries to O(log N + K).)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

namespace range_tree {

    struct Point { int x, y, id; };

    class RangeTree2D {
        int n;
        std::vector<Point> pts;                      // sorted by x
        std::vector<std::vector<std::pair<int,int>>> node_y; // (y, index into pts)

        void build(int node, int lo, int hi) {
            if (lo == hi) { node_y[node] = {{pts[lo].y, lo}}; return; }
            int mid = (lo + hi) / 2;
            build(2 * node, lo, mid);
            build(2 * node + 1, mid + 1, hi);
            auto& L = node_y[2 * node];
            auto& R = node_y[2 * node + 1];
            node_y[node].resize(L.size() + R.size());
            std::merge(L.begin(), L.end(), R.begin(), R.end(), node_y[node].begin());
        }

        // Points index range [l, r] in x-sorted order
        template <typename F>
        void visit(int node, int lo, int hi, int l, int r, F&& f) const {
            if (r < lo || hi < l) return;
            if (l <= lo && hi <= r) { f(node); return; }
            int mid = (lo + hi) / 2;
            visit(2 * node, lo, mid, l, r, f);
            visit(2 * node + 1, mid + 1, hi, l, r, f);
        }

        std::pair<int,int> x_index_range(int x1, int x2) const {
            auto cmp_lo = [](const Point& p, int v) { return p.x < v; };
            auto cmp_hi = [](int v, const Point& p) { return v < p.x; };
            int l = static_cast<int>(std::lower_bound(pts.begin(), pts.end(), x1, cmp_lo) - pts.begin());
            int r = static_cast<int>(std::upper_bound(pts.begin(), pts.end(), x2, cmp_hi) - pts.begin()) - 1;
            return {l, r};
        }

    public:
        explicit RangeTree2D(std::vector<Point> p) : n(static_cast<int>(p.size())), pts(std::move(p)) {
            std::sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) { return a.x < b.x; });
            node_y.assign(4 * std::max(1, n), {});
            if (n) build(1, 0, n - 1);
        }

        int count(int x1, int x2, int y1, int y2) const {
            auto [l, r] = x_index_range(x1, x2);
            if (n == 0 || l > r) return 0;
            int total = 0;
            visit(1, 0, n - 1, l, r, [&](int node) {
                const auto& v = node_y[node];
                auto a = std::lower_bound(v.begin(), v.end(), std::make_pair(y1, -1));
                auto b = std::upper_bound(v.begin(), v.end(), std::make_pair(y2, n));
                total += static_cast<int>(b - a);
            });
            return total;
        }

        std::vector<Point> report(int x1, int x2, int y1, int y2) const {
            std::vector<Point> out;
            auto [l, r] = x_index_range(x1, x2);
            if (n == 0 || l > r) return out;
            visit(1, 0, n - 1, l, r, [&](int node) {
                const auto& v = node_y[node];
                auto a = std::lower_bound(v.begin(), v.end(), std::make_pair(y1, -1));
                for (; a != v.end() && a->first <= y2; ++a) out.push_back(pts[a->second]);
            });
            return out;
        }
    };

} // namespace range_tree

int main() {
    using namespace range_tree;
    std::cout << "============================================================\n";
    std::cout << "       2D Range Tree (Orthogonal Range Counting/Reporting)\n";
    std::cout << "============================================================\n\n";

    std::vector<Point> small = {
        {2, 3, 0}, {5, 4, 1}, {9, 6, 2}, {4, 7, 3}, {8, 1, 4}, {7, 2, 5}, {3, 9, 6}, {6, 5, 7}
    };
    RangeTree2D rt(small);
    std::cout << "Query [3,8] x [2,6]:\n";
    auto rep = rt.report(3, 8, 2, 6);
    std::sort(rep.begin(), rep.end(), [](const Point& a, const Point& b) { return a.id < b.id; });
    for (auto& p : rep) std::cout << "  P" << p.id << " (" << p.x << ", " << p.y << ")\n";
    std::cout << "Count = " << rt.count(3, 8, 2, 6) << " (expected 3: P1, P5, P7)\n";

    // Randomized verification against brute force
    std::mt19937 rng(99);
    std::vector<Point> pts;
    for (int i = 0; i < 3000; ++i) pts.push_back({int(rng() % 1000), int(rng() % 1000), i});
    RangeTree2D big(pts);
    bool ok = true;
    for (int q = 0; q < 2000 && ok; ++q) {
        int x1 = rng() % 1000, x2 = rng() % 1000, y1 = rng() % 1000, y2 = rng() % 1000;
        if (x1 > x2) std::swap(x1, x2);
        if (y1 > y2) std::swap(y1, y2);
        int brute = 0;
        for (auto& p : pts) if (p.x >= x1 && p.x <= x2 && p.y >= y1 && p.y <= y2) ++brute;
        if (brute != big.count(x1, x2, y1, y2) ||
            brute != static_cast<int>(big.report(x1, x2, y1, y2).size())) ok = false;
    }
    std::cout << "\nRandom verification (3000 points, 2000 queries): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
