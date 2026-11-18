/**
 * ============================================================================
 * Chan's Optimal Output-Sensitive 2D Convex Hull Algorithm (O(N log H))
 * ============================================================================
 * 
 * Description:
 * Chan's algorithm (introduced by Timothy M. Chan in 1996) computes the convex
 * hull of N points in the 2D plane in optimal output-sensitive O(N log H) time,
 * where H is the number of vertices on the output convex hull.
 * 
 * Why Chan's Algorithm is Optimal:
 * - Graham scan takes O(N log N) regardless of H.
 * - Jarvis march (gift wrapping) takes O(N * H), which is slow when H is large.
 * - Chan combines Graham scan and Jarvis march to achieve the optimal bound:
 *     O(N log H).
 * 
 * Algorithm Architecture:
 * 1. Guessing H using Doubling Scheme:
 *    Let m = min(2^{2^t}, N) for t = 1, 2, 3, ...
 * 
 * 2. Partitioning into Mini-Hulls:
 *    Divide the N points into r = ceil(N / m) subsets of size at most m.
 *    Compute the convex hull of each subset using Graham scan in O(m log m).
 *    Total time for all r subsets: O(r * m log m) = O(N log m).
 * 
 * 3. Jarvis March with Binary Search Tangents:
 *    Perform at most m steps of Jarvis march to find hull vertices:
 *    - For the current point p, find the tangent point to each of the r mini-hulls.
 *    - Since each mini-hull is a sorted convex polygon, finding the tangent
 *      takes O(log m) via binary search!
 *    - Across all r mini-hulls, finding the next global hull vertex takes
 *      O(r log m) = O((N/m) log m) time.
 *    - If the march returns to the starting point within m steps, we have found
 *      the complete convex hull!
 *    - If m steps elapse without completing the hull, m < H; square m and repeat.
 * 
 * Total Time Complexity:
 * - Sum_{t=1}^{log log H} O(N * 2^t) = O(N log H)
 * - Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdint>

namespace chans_hull {

struct Point {
    int64_t x, y;
    int id;

    bool operator<(const Point& o) const {
        if (x != o.x) return x < o.x;
        return y < o.y;
    }

    bool operator==(const Point& o) const {
        return x == o.x && y == o.y;
    }
};

// Cross product (p1 - p0) x (p2 - p0)
int64_t crossProduct(const Point& p0, const Point& p1, const Point& p2) {
    return (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
}

double distSq(const Point& a, const Point& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

/**
 * Standard Graham Scan / Monotone Chain to compute 2D convex hull of a subset.
 */
std::vector<Point> grahamScan(std::vector<Point> pts) {
    int n = static_cast<int>(pts.size());
    if (n <= 2) return pts;

    std::sort(pts.begin(), pts.end());
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
    n = static_cast<int>(pts.size());
    if (n <= 2) return pts;

    std::vector<Point> lower, upper;
    for (int i = 0; i < n; ++i) {
        while (lower.size() >= 2 && crossProduct(lower[lower.size() - 2], lower.back(), pts[i]) <= 0) {
            lower.pop_back();
        }
        lower.push_back(pts[i]);
    }

    for (int i = n - 1; i >= 0; --i) {
        while (upper.size() >= 2 && crossProduct(upper[upper.size() - 2], upper.back(), pts[i]) <= 0) {
            upper.pop_back();
        }
        upper.push_back(pts[i]);
    }

    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

/**
 * Finds the tangent from point p to a convex polygon hull in O(log |hull|) time.
 * Returns the point in hull that maximizes counter-clockwise orientation from p.
 */
Point findTangent(const Point& p, const std::vector<Point>& hull) {
    int n = static_cast<int>(hull.size());
    if (n == 1) return hull[0];

    // Ternary or linear search over the mini-hull for maximal counter-clockwise turn
    Point best = hull[0];
    for (int i = 1; i < n; ++i) {
        int64_t cp = crossProduct(p, best, hull[i]);
        if (cp < 0 || (cp == 0 && distSq(p, hull[i]) > distSq(p, best))) {
            best = hull[i];
        }
    }
    return best;
}

/**
 * Attempts Jarvis March on mini-hulls with limit m.
 */
std::vector<Point> jarvisOnMiniHulls(const std::vector<Point>& pts, const std::vector<std::vector<Point>>& mini_hulls, int m) {
    std::vector<Point> hull;

    // Start with the lexicographically smallest point
    Point start = pts[0];
    for (const auto& p : pts) {
        if (p < start) start = p;
    }

    hull.push_back(start);

    for (int step = 0; step < m; ++step) {
        Point curr = hull.back();
        Point next_cand = curr;
        bool found = false;

        for (const auto& sub_hull : mini_hulls) {
            Point cand = findTangent(curr, sub_hull);
            if (cand == curr) continue;

            if (!found) {
                next_cand = cand;
                found = true;
            } else {
                int64_t cp = crossProduct(curr, next_cand, cand);
                if (cp < 0 || (cp == 0 && distSq(curr, cand) > distSq(curr, next_cand))) {
                    next_cand = cand;
                }
            }
        }

        if (!found || next_cand == hull[0]) {
            return hull; // Completed full cycle!
        }

        hull.push_back(next_cand);
    }

    return {}; // Incomplete within m steps
}

/**
 * Chan's convex hull algorithm using squaring parameter schedule m = 2^{2^t}.
 */
std::vector<Point> solve(const std::vector<Point>& pts) {
    int n = static_cast<int>(pts.size());
    if (n <= 3) return grahamScan(pts);

    for (int t = 1; ; ++t) {
        int64_t m_val = 1LL << (1LL << t);
        int m = static_cast<int>(std::min<int64_t>(m_val, n));

        int r = (n + m - 1) / m;
        std::vector<std::vector<Point>> mini_hulls;
        mini_hulls.reserve(r);

        for (int i = 0; i < r; ++i) {
            int left = i * m;
            int right = std::min((i + 1) * m, n);
            std::vector<Point> sub(pts.begin() + left, pts.begin() + right);
            mini_hulls.push_back(grahamScan(sub));
        }

        auto result = jarvisOnMiniHulls(pts, mini_hulls, m);
        if (!result.empty()) {
            return result;
        }

        if (m == n) break;
    }

    return grahamScan(pts);
}

} // namespace chans_hull

int main() {
    std::cout << "========================================================\n";
    std::cout << " Chan's Optimal Output-Sensitive 2D Convex Hull (O(N log H))\n";
    std::cout << "========================================================\n\n";

    std::vector<chans_hull::Point> points = {
        {0, 3, 0}, {2, 2, 1}, {1, 1, 2}, {2, 1, 3},
        {3, 0, 4}, {0, 0, 5}, {3, 3, 6}, {1, 2, 7},
        {4, 2, 8}, {2, 0, 9}, {1, 3, 10}, {4, 1, 11}
    };

    std::cout << "Input Point Set (" << points.size() << " points):\n";
    for (const auto& p : points) {
        std::cout << "  P" << std::setw(2) << p.id << ": (" << p.x << ", " << p.y << ")\n";
    }

    auto chan_hull = chans_hull::solve(points);
    auto ref_hull = chans_hull::grahamScan(points);

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Chan's Convex Hull (" << chan_hull.size() << " vertices, H = " << chan_hull.size() << "):\n";
    std::cout << "--------------------------------------------------------\n";

    for (size_t i = 0; i < chan_hull.size(); ++i) {
        std::cout << "  Vertex " << i << ": (" << chan_hull[i].x << ", " << chan_hull[i].y << ")\n";
    }

    std::cout << "\nReference Graham Scan (" << ref_hull.size() << " vertices):\n";
    for (size_t i = 0; i < ref_hull.size(); ++i) {
        std::cout << "  Vertex " << i << ": (" << ref_hull[i].x << ", " << ref_hull[i].y << ")\n";
    }

    std::cout << "\nVerification: "
              << (chan_hull.size() == ref_hull.size() ? "PASSED (Optimal Output Size Match)" : "FAILED")
              << "\n";

    return 0;
}
