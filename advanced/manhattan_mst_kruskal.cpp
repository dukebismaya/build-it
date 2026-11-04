/**
 * @file manhattan_mst_kruskal.cpp
 * @brief Computes the Manhattan Minimum Spanning Tree of N points in O(N log N) time.
 *
 * Problem:
 * Given N points in a 2D plane, find a spanning tree connecting all points with minimum
 * total Manhattan distance: dist(P, Q) = |P.x - Q.x| + |P.y - Q.y|.
 *
 * Theoretical Key Insight (Guibas & Stolfi 1983):
 * For any point P, in each of the 8 45-degree octants around P, only the single closest
 * point can possibly be an edge in the MST.
 * Thus, out of all O(N^2) edges, at most 4N edges are candidates for the MST!
 *
 * Algorithm:
 * 1. By 4 geometric coordinate transformations (rotations/reflections), reduce the 8 octants
 *    to a single canonical octant: R1 = {Q : Q.x >= P.x, Q.y - P.y >= 0, Q.y - P.y <= Q.x - P.x}.
 * 2. In this octant: dist(P, Q) = (Q.x + Q.y) - (P.x + P.y).
 *    Minimizing dist(P, Q) is equivalent to minimizing (Q.x + Q.y) subject to Q.x - Q.y >= P.x - P.y.
 * 3. Use sweep-line with coordinate compression on (x - y) and a Fenwick tree (Binary Indexed Tree)
 *    to find the candidate edge for each point in O(N log N).
 * 4. Run Kruskal's algorithm on the resulting <= 4N edges.
 *
 * Time Complexity: O(N log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iomanip>

struct Point {
    long long x;
    long long y;
    int id;
};

struct Edge {
    int u;
    int v;
    long long weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class FenwickTreeMin {
private:
    int n;
    static constexpr long long INF = 2e18;
    std::vector<long long> treeVal;
    std::vector<int> treeId;

public:
    explicit FenwickTreeMin(int size) : n(size), treeVal(size + 1, INF), treeId(size + 1, -1) {}

    void update(int idx, long long val, int id) {
        for (; idx <= n; idx += idx & -idx) {
            if (val < treeVal[idx]) {
                treeVal[idx] = val;
                treeId[idx] = id;
            }
        }
    }

    std::pair<long long, int> query(int idx) const {
        long long minVal = INF;
        int minId = -1;
        for (; idx > 0; idx -= idx & -idx) {
            if (treeVal[idx] < minVal) {
                minVal = treeVal[idx];
                minId = treeId[idx];
            }
        }
        return {minVal, minId};
    }
};

class DSU {
private:
    std::vector<int> parent;
    int components;

public:
    explicit DSU(int n) : parent(n), components(n) {
        std::iota(parent.begin(), parent.end(), 0);
    }

    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }

    bool unite(int i, int j) {
        int rootI = find(i);
        int rootJ = find(j);
        if (rootI != rootJ) {
            parent[rootI] = rootJ;
            components--;
            return true;
        }
        return false;
    }

    int getComponents() const { return components; }
};

class ManhattanMST {
private:
    static void addOctantEdges(std::vector<Point>& pts, std::vector<Edge>& edges) {
        int n = static_cast<int>(pts.size());

        // Sort descending by x
        std::sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) {
            if (a.x != b.x) return a.x > b.x;
            return a.y > b.y;
        });

        // Values of (x - y) for coordinate compression
        std::vector<long long> diffs(n);
        for (int i = 0; i < n; ++i) {
            diffs[i] = pts[i].x - pts[i].y;
        }
        std::sort(diffs.begin(), diffs.end());
        diffs.erase(std::unique(diffs.begin(), diffs.end()), diffs.end());

        FenwickTreeMin bit(static_cast<int>(diffs.size()));

        for (int i = 0; i < n; ++i) {
            long long d = pts[i].x - pts[i].y;
            // 1-based index in compressed coordinates
            int pos = static_cast<int>(std::lower_bound(diffs.begin(), diffs.end(), d) - diffs.begin()) + 1;

            auto [minSum, bestId] = bit.query(pos);
            if (bestId != -1) {
                long long dist = minSum - (pts[i].x + pts[i].y);
                edges.push_back({pts[i].id, bestId, dist});
            }

            bit.update(pos, pts[i].x + pts[i].y, pts[i].id);
        }
    }

public:
    static std::pair<long long, std::vector<Edge>> computeMST(std::vector<Point> pts) {
        int n = static_cast<int>(pts.size());
        if (n <= 1) return {0, {}};

        for (int i = 0; i < n; ++i) {
            pts[i].id = i;
        }

        std::vector<Edge> candidateEdges;

        // 4 transformations cover all 8 octants
        for (int step = 0; step < 4; ++step) {
            if (step == 1 || step == 3) {
                for (auto& p : pts) std::swap(p.x, p.y);
            } else if (step == 2) {
                for (auto& p : pts) p.x = -p.x;
            }
            addOctantEdges(pts, candidateEdges);
        }

        // Run Kruskal's algorithm
        std::sort(candidateEdges.begin(), candidateEdges.end());
        DSU dsu(n);
        long long totalWeight = 0;
        std::vector<Edge> mstEdges;

        for (const auto& e : candidateEdges) {
            if (dsu.unite(e.u, e.v)) {
                totalWeight += e.weight;
                mstEdges.push_back(e);
                if (static_cast<int>(mstEdges.size()) == n - 1) break;
            }
        }

        return {totalWeight, mstEdges};
    }
};

int main() {
    std::cout << "=== Manhattan Minimum Spanning Tree (O(N log N)) ===\n\n";

    // 5 2D Points
    std::vector<Point> points = {
        {0, 0, 0},
        {2, 2, 1},
        {3, 10, 2},
        {5, 2, 3},
        {7, 0, 4}
    };

    std::cout << "Input Points:\n";
    for (size_t i = 0; i < points.size(); ++i) {
        std::cout << "  Point " << i << ": (" << points[i].x << ", " << points[i].y << ")\n";
    }

    auto [mstWeight, mstEdges] = ManhattanMST::computeMST(points);

    std::cout << "\nManhattan Minimum Spanning Tree:\n";
    std::cout << "  Total MST Weight: " << mstWeight << '\n';
    std::cout << "  Selected Edges:\n";
    for (const auto& e : mstEdges) {
        std::cout << "    Edge (" << e.u << " - " << e.v << ") : Manhattan Distance = " << e.weight << '\n';
    }

    return 0;
}
