/**
 * ============================================================================
 * Closest Pair of 2D Points (Divide-and-Conquer Algorithm)
 * ============================================================================
 * 
 * Description:
 * Given a set of N points in the 2D Euclidean plane, find the pair of points
 * with the smallest Euclidean distance between them.
 * 
 * Divide-and-Conquer Strategy:
 * 1. Initial Sort:
 *    Sort all points by x-coordinate: P_x.
 * 
 * 2. Divide:
 *    Split points at the median x-coordinate into left set P_L and right set P_R.
 * 
 * 3. Conquer:
 *    Recursively find the closest distance in the left half (d_L) and in the
 *    right half (d_R). Let delta = min(d_L, d_R).
 * 
 * 4. Combine / Strip Check:
 *    Points from P_L and P_R might be closer than delta to each other.
 *    Any such cross-boundary pair must both lie within the vertical strip:
 *      |x - x_mid| < delta.
 *    Filter points in this strip and sort them by y-coordinate.
 * 
 * 5. Geometric Constant-Bound Lemma:
 *    For each point in the y-sorted strip, we only need to compare it with
 *    subsequent points whose y-difference is < delta.
 *    In a delta x 2*delta box, at most 7 points can coexist without violating
 *    the condition that pairwise distances in each half are >= delta.
 *    Therefore, the inner loop executes at most 7 times per point!
 * 
 * Time Complexity:
 * - O(N log^2 N) standard or O(N log N) with merge-sort on y
 * - Space Complexity: O(N) auxiliary
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <limits>

namespace closest_pair {

struct Point {
    double x, y;
    int id;
};

struct Result {
    double distance;
    Point p1, p2;
};

double dist(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

/**
 * Brute force solution for base case and verification.
 */
Result bruteForce(const std::vector<Point>& pts, int left, int right) {
    double min_d = std::numeric_limits<double>::max();
    Result best{min_d, {0, 0, -1}, {0, 0, -1}};

    for (int i = left; i <= right; ++i) {
        for (int j = i + 1; j <= right; ++j) {
            double d = dist(pts[i], pts[j]);
            if (d < best.distance) {
                best = {d, pts[i], pts[j]};
            }
        }
    }
    return best;
}

Result closestPairRec(std::vector<Point>& pts, int left, int right) {
    int count = right - left + 1;
    if (count <= 3) {
        return bruteForce(pts, left, right);
    }

    int mid = left + (right - left) / 2;
    double mid_x = pts[mid].x;

    Result res_l = closestPairRec(pts, left, mid);
    Result res_r = closestPairRec(pts, mid + 1, right);

    Result best = (res_l.distance < res_r.distance) ? res_l : res_r;
    double delta = best.distance;

    // Build strip of points within delta distance from mid_x
    std::vector<Point> strip;
    for (int i = left; i <= right; ++i) {
        if (std::abs(pts[i].x - mid_x) < delta) {
            strip.push_back(pts[i]);
        }
    }

    // Sort strip points by y-coordinate
    std::sort(strip.begin(), strip.end(), [](const Point& a, const Point& b) {
        return a.y < b.y;
    });

    // Check at most 7 neighbors in strip
    int strip_sz = static_cast<int>(strip.size());
    for (int i = 0; i < strip_sz; ++i) {
        for (int j = i + 1; j < strip_sz && (strip[j].y - strip[i].y) < delta; ++j) {
            double d = dist(strip[i], strip[j]);
            if (d < best.distance) {
                best = {d, strip[i], strip[j]};
                delta = d;
            }
        }
    }

    return best;
}

Result solve(std::vector<Point> pts) {
    std::sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) {
        if (a.x != b.x) return a.x < b.x;
        return a.y < b.y;
    });

    return closestPairRec(pts, 0, static_cast<int>(pts.size()) - 1);
}

} // namespace closest_pair

int main() {
    std::cout << "========================================================\n";
    std::cout << " 2D Closest Pair of Points (Divide-and-Conquer)\n";
    std::cout << "========================================================\n\n";

    std::vector<closest_pair::Point> points = {
        {2.0, 3.0, 0},   {12.0, 30.0, 1}, {40.0, 50.0, 2},
        {5.0, 1.0, 3},   {12.0, 10.0, 4}, {3.0, 4.0, 5},
        {1.0, 1.0, 6},   {2.5, 3.2, 7},   {15.0, 28.0, 8},
        {22.0, 21.0, 9}, {7.0, 19.0, 10}, {9.0, 2.0, 11}
    };

    std::cout << "Input Point Set (" << points.size() << " points):\n";
    for (const auto& p : points) {
        std::cout << "  Point " << std::setw(2) << p.id
                  << ": (" << std::fixed << std::setprecision(1) << p.x
                  << ", " << p.y << ")\n";
    }

    auto fast_res = closest_pair::solve(points);
    auto brute_res = closest_pair::bruteForce(points, 0, static_cast<int>(points.size()) - 1);

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Results & Verification:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "Divide-and-Conquer Result:\n";
    std::cout << "  Closest Points: P" << fast_res.p1.id << " (" << fast_res.p1.x << ", " << fast_res.p1.y << ") and P"
              << fast_res.p2.id << " (" << fast_res.p2.x << ", " << fast_res.p2.y << ")\n";
    std::cout << "  Distance: " << std::setprecision(6) << fast_res.distance << "\n\n";

    std::cout << "Brute Force O(N^2) Result:\n";
    std::cout << "  Distance: " << std::setprecision(6) << brute_res.distance << "\n";
    std::cout << "  Match? "
              << (std::abs(fast_res.distance - brute_res.distance) < 1e-9 ? "YES (EXACT MATCH)" : "MISMATCH")
              << "\n";

    return 0;
}
