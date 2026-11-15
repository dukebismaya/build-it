/*
 * ============================================================================
 *  Smallest Enclosing Circle (Welzl's Algorithm)
 * ============================================================================
 *
 *  Given a set of N points in the plane, find the smallest circle that
 *  contains all of them.
 *
 *  Welzl's Algorithm (1991):
 *  - A randomized incremental algorithm running in expected O(N) time.
 *  - Works by adding points one at a time. If a newly added point lies
 *    outside the current enclosing circle, the circle must pass through
 *    that point. Recurse with this constraint.
 *  - The boundary set B (points the circle must pass through) has at most
 *    3 points. The base cases construct a circle from 0, 1, 2, or 3 points.
 *
 *  Properties:
 *  - The smallest enclosing circle is unique.
 *  - It is determined by at most 3 points on its boundary.
 *  - Expected O(N) time due to the randomized permutation.
 *
 *  Time Complexity:  O(N) expected
 *  Space Complexity: O(N) for recursion / iteration
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>
#include <cstdint>

namespace geometry {

constexpr double EPS = 1e-10;

struct Point {
    double x, y;
};

struct Circle {
    Point center;
    double radius;
};

double dist(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool isInside(const Circle& c, const Point& p) {
    return dist(c.center, p) <= c.radius + EPS;
}

Circle makeCircleFrom1(const Point& a) {
    return {{a.x, a.y}, 0.0};
}

Circle makeCircleFrom2(const Point& a, const Point& b) {
    Point center = {(a.x + b.x) / 2.0, (a.y + b.y) / 2.0};
    return {center, dist(a, b) / 2.0};
}

Circle makeCircleFrom3(const Point& a, const Point& b, const Point& c) {
    double ax = b.x - a.x, ay = b.y - a.y;
    double bx = c.x - a.x, by = c.y - a.y;
    double D = 2.0 * (ax * by - ay * bx);

    if (std::abs(D) < EPS) {
        /* Collinear: return circle through the two farthest points */
        double d1 = dist(a, b);
        double d2 = dist(b, c);
        double d3 = dist(a, c);
        if (d1 >= d2 && d1 >= d3) return makeCircleFrom2(a, b);
        if (d2 >= d1 && d2 >= d3) return makeCircleFrom2(b, c);
        return makeCircleFrom2(a, c);
    }

    double ux = (by * (ax * ax + ay * ay) - ay * (bx * bx + by * by)) / D;
    double uy = (ax * (bx * bx + by * by) - bx * (ax * ax + ay * ay)) / D;

    Point center = {a.x + ux, a.y + uy};
    return {center, dist(center, a)};
}

/* Trivial circle from boundary set (0 to 3 points) */
Circle trivialCircle(const std::vector<Point>& boundary) {
    if (boundary.empty()) return {{0, 0}, 0};
    if (boundary.size() == 1) return makeCircleFrom1(boundary[0]);
    if (boundary.size() == 2) return makeCircleFrom2(boundary[0], boundary[1]);
    return makeCircleFrom3(boundary[0], boundary[1], boundary[2]);
}

/*
 * Welzl's algorithm (iterative version to avoid stack overflow).
 * We implement the standard recursive version with an explicit
 * shuffled input, which gives expected O(N) behavior.
 */
Circle welzlHelper(std::vector<Point>& points, std::vector<Point> boundary, int n) {
    if (n == 0 || boundary.size() == 3) {
        return trivialCircle(boundary);
    }

    int idx = n - 1;
    Point p = points[idx];

    Circle circle = welzlHelper(points, boundary, n - 1);

    if (isInside(circle, p)) {
        return circle;
    }

    boundary.push_back(p);
    return welzlHelper(points, boundary, n - 1);
}

Circle smallestEnclosingCircle(std::vector<Point> points) {
    /* Shuffle for expected O(N) */
    std::mt19937 rng(42);
    std::shuffle(points.begin(), points.end(), rng);

    return welzlHelper(points, {}, static_cast<int>(points.size()));
}

/* Brute force O(N^4) for verification on small inputs */
Circle bruteForce(const std::vector<Point>& pts) {
    int n = static_cast<int>(pts.size());
    Circle best = {{0, 0}, 1e18};

    /* Try all single-point circles */
    for (int i = 0; i < n; ++i) {
        Circle c = makeCircleFrom1(pts[i]);
        bool valid = true;
        for (int j = 0; j < n; ++j) {
            if (!isInside(c, pts[j])) { valid = false; break; }
        }
        if (valid && c.radius < best.radius) best = c;
    }

    /* Try all 2-point circles */
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            Circle c = makeCircleFrom2(pts[i], pts[j]);
            bool valid = true;
            for (int k = 0; k < n; ++k) {
                if (!isInside(c, pts[k])) { valid = false; break; }
            }
            if (valid && c.radius < best.radius) best = c;
        }
    }

    /* Try all 3-point circles */
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int k = j + 1; k < n; ++k) {
                Circle c = makeCircleFrom3(pts[i], pts[j], pts[k]);
                bool valid = true;
                for (int m = 0; m < n; ++m) {
                    if (!isInside(c, pts[m])) { valid = false; break; }
                }
                if (valid && c.radius < best.radius) best = c;
            }
        }
    }

    return best;
}

} // namespace geometry

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Smallest Enclosing Circle (Welzl's Algorithm)\n";
    std::cout << "========================================================\n\n";

    std::vector<geometry::Point> points = {
        {0, 0}, {1, 0}, {0, 1}, {1, 1}, {0.5, 0.5},
        {-1, 0.5}, {2, 0.3}, {0.7, -0.8}
    };

    int n = static_cast<int>(points.size());
    std::cout << "Input Points (" << n << "):\n";
    for (int i = 0; i < n; ++i) {
        std::cout << "  P" << i << " = (" << std::fixed << std::setprecision(2)
                  << points[i].x << ", " << points[i].y << ")\n";
    }

    auto result = geometry::smallestEnclosingCircle(points);
    auto brute = geometry::bruteForce(points);

    std::cout << "\nWelzl's Result:\n";
    std::cout << "  Center = (" << std::setprecision(6) << result.center.x
              << ", " << result.center.y << ")\n";
    std::cout << "  Radius = " << result.radius << "\n";

    std::cout << "\nBrute Force Result:\n";
    std::cout << "  Center = (" << brute.center.x << ", " << brute.center.y << ")\n";
    std::cout << "  Radius = " << brute.radius << "\n";

    std::cout << "\nRadius match: "
              << (std::abs(result.radius - brute.radius) < 1e-6 ? "YES" : "NO") << "\n";

    /* Verify all points are inside */
    std::cout << "\nContainment Check:\n";
    bool allInside = true;
    for (int i = 0; i < n; ++i) {
        bool inside = geometry::isInside(result, points[i]);
        if (!inside) allInside = false;
        double d = geometry::dist(result.center, points[i]);
        std::cout << "  P" << i << ": dist=" << std::setprecision(6) << d
                  << "  inside=" << (inside ? "yes" : "NO") << "\n";
    }
    std::cout << "\nAll points enclosed: " << (allInside ? "PASSED" : "FAILED") << "\n";

    return 0;
}
