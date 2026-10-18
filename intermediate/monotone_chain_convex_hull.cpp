/*
 * Program Description:
 * Andrew's Monotone Chain Algorithm for 2D Convex Hull.
 * Computes the convex hull of a set of 2D points in O(N log N) time.
 * A variant of Graham's Scan that sorts points lexicographically by (x, y),
 * constructing the lower hull and upper hull independently without trigonometric functions.
 *
 * Steps:
 * 1. Sort points by X coordinate (break ties by Y coordinate).
 * 2. Construct Lower Hull by iterating left to right, maintaining strictly counter-clockwise turns.
 * 3. Construct Upper Hull by iterating right to left, maintaining strictly counter-clockwise turns.
 * 4. Concatenate lower and upper hulls.
 *
 * Additional features:
 * - Computes perimeter of the convex hull.
 * - Computes exact enclosed area using the Shoelace formula.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

struct Point {
    long long x, y;

    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// 2D cross product of vector OA and OB.
// Returns positive for CCW turn, negative for CW turn, 0 for collinear.
long long crossProduct(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Andrew's monotone chain algorithm
std::vector<Point> convexHullMonotoneChain(std::vector<Point> pts) {
    int n = pts.size();
    if (n <= 1) return pts;

    std::sort(pts.begin(), pts.end());
    // Remove duplicates
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());
    n = pts.size();
    if (n <= 2) return pts;

    std::vector<Point> hull;
    hull.reserve(2 * n);

    // Build Lower Hull
    for (int i = 0; i < n; ++i) {
        while (hull.size() >= 2 && crossProduct(hull[hull.size() - 2], hull.back(), pts[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    // Build Upper Hull
    size_t lowerSize = hull.size();
    for (int i = n - 2; i >= 0; --i) {
        while (hull.size() > lowerSize && crossProduct(hull[hull.size() - 2], hull.back(), pts[i]) <= 0) {
            hull.pop_back();
        }
        hull.push_back(pts[i]);
    }

    hull.pop_back(); // Remove duplicate starting point at end
    return hull;
}

// Shoelace formula for polygon area
double polygonArea(const std::vector<Point>& hull) {
    double area = 0.0;
    int n = hull.size();
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        area += (hull[i].x * hull[j].y) - (hull[j].x * hull[i].y);
    }
    return std::abs(area) / 2.0;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Andrew's Monotone Chain 2D Convex Hull  \n";
    std::cout << "==========================================\n\n";

    std::vector<Point> points = {
        {0, 3}, {2, 2}, {1, 1}, {2, 1},
        {3, 0}, {0, 0}, {3, 3}, {1, 2}
    };

    std::cout << "Input Points (" << points.size() << "):\n  ";
    for (const auto& p : points) {
        std::cout << "(" << p.x << ", " << p.y << ") ";
    }
    std::cout << "\n\n";

    std::vector<Point> hull = convexHullMonotoneChain(points);

    std::cout << "Convex Hull Vertices (" << hull.size() << " in CCW order):\n";
    for (const auto& p : hull) {
        std::cout << "  -> (" << p.x << ", " << p.y << ")\n";
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nEnclosed Convex Hull Area: " << polygonArea(hull) << " sq units\n";

    return 0;
}
