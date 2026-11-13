/**
 * @file jarvis_march_convex_hull.cpp
 * @brief Jarvis March (Gift Wrapping) 2D Convex Hull algorithm in C++17.
 *
 * Overview (R. A. Jarvis, 1973):
 *   Computes the convex hull of a set of 2D points using an output-sensitive algorithm.
 *   The algorithm conceptually "wraps" a ribbon around the set of points starting from
 *   the leftmost point until it returns to the start.
 *
 * Geometric Principle:
 *   Starting from the current hull point P, the next hull point Q is the point that
 *   appears most counter-clockwise to all other points R. This is evaluated via the
 *   2D cross product of vectors (Q - P) and (R - P):
 *     cross = (Q.x - P.x) * (R.y - P.y) - (Q.y - P.y) * (R.x - P.x)
 *   If cross > 0: R is strictly more counter-clockwise than Q.
 *   If cross == 0: Collinear (choose the point furthest from P).
 *
 * Complexity:
 *   - Time: O(N * H) where N is total points and H is the number of hull vertices.
 *     (Optimal for small H, e.g. H = O(1)).
 *   - Space: O(H) for output hull vertices.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace geometry {

struct Point {
    double x;
    double y;
    int id;

    bool operator==(const Point& o) const {
        return id == o.id || (std::abs(x - o.x) < 1e-9 && std::abs(y - o.y) < 1e-9);
    }
};

/**
 * @brief Computes 2D cross product (Q - P) x (R - P).
 * @return >0 for CCW turn, <0 for CW turn, 0 for collinear.
 */
double crossProduct(const Point& p, const Point& q, const Point& r) {
    return (q.x - p.x) * (r.y - p.y) - (q.y - p.y) * (r.x - p.x);
}

double distSq(const Point& a, const Point& b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

class JarvisMarch {
public:
    static std::vector<Point> computeConvexHull(const std::vector<Point>& points) {
        int n = static_cast<int>(points.size());
        if (n < 3) return points;

        // Step 1: Find leftmost point (smallest x, break ties with smallest y)
        int startIdx = 0;
        for (int i = 1; i < n; ++i) {
            if (points[i].x < points[startIdx].x ||
               (std::abs(points[i].x - points[startIdx].x) < 1e-9 && points[i].y < points[startIdx].y)) {
                startIdx = i;
            }
        }

        std::vector<Point> hull;
        int p = startIdx;

        // Step 2: Wrap around until returning to startIdx
        do {
            hull.push_back(points[p]);
            int q = (p + 1) % n;

            for (int r = 0; r < n; ++r) {
                double cp = crossProduct(points[p], points[q], points[r]);
                if (cp > 1e-9) {
                    q = r;
                } else if (std::abs(cp) < 1e-9 && distSq(points[p], points[r]) > distSq(points[p], points[q])) {
                    // Collinear: pick furthest point
                    q = r;
                }
            }
            p = q;
        } while (p != startIdx);

        return hull;
    }

    static double computePolygonArea(const std::vector<Point>& hull) {
        double area = 0.0;
        size_t m = hull.size();
        for (size_t i = 0; i < m; ++i) {
            size_t j = (i + 1) % m;
            area += (hull[i].x * hull[j].y) - (hull[j].x * hull[i].y);
        }
        return std::abs(area) / 2.0;
    }
};

} // namespace geometry

int main() {
    std::cout << "========================================\n";
    std::cout << "      JARVIS MARCH 2D CONVEX HULL       \n";
    std::cout << "         (Gift Wrapping O(N*H))         \n";
    std::cout << "========================================\n\n";

    std::vector<geometry::Point> points = {
        {0.0, 3.0, 0},
        {2.0, 2.0, 1},
        {1.0, 1.0, 2},
        {2.0, 1.0, 3},
        {3.0, 0.0, 4},
        {0.0, 0.0, 5},
        {3.0, 3.0, 6},
        {1.5, 1.5, 7}  // Interior point
    };

    std::cout << "Input 2D Points (" << points.size() << " total):\n";
    for (const auto& pt : points) {
        std::cout << "  Point " << pt.id << ": (" << pt.x << ", " << pt.y << ")\n";
    }

    auto hull = geometry::JarvisMarch::computeConvexHull(points);
    double area = geometry::JarvisMarch::computePolygonArea(hull);

    std::cout << "\nComputed Convex Hull (" << hull.size() << " boundary vertices in CCW order):\n";
    std::cout << std::left << std::setw(8) << "Index"
              << std::setw(12) << "Point ID"
              << "Coordinates\n";
    std::cout << std::string(36, '-') << "\n";

    for (size_t i = 0; i < hull.size(); ++i) {
        std::cout << std::left << std::setw(8) << (i + 1)
                  << "Point " << std::setw(6) << hull[i].id
                  << "(" << hull[i].x << ", " << hull[i].y << ")\n";
    }

    std::cout << "\nEnclosed Convex Hull Area: " << std::fixed << std::setprecision(2) << area << "\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
