/**
 * @file halfplane_intersection_geometry.cpp
 * @brief Computes the Convex Polygon formed by the Intersection of N Halfplanes in O(N log N) time.
 *
 * Theory:
 * A halfplane is defined by a directed line segment P -> Q. The valid region lies to the
 * left of the directed line: (Q - P) x (X - P) >= 0.
 *
 * S&I Algorithm (Angle Sorting + Deque):
 * 1. Sort halfplanes by their polar angle in [ -pi, pi ]. For identical angles, keep the innermost line.
 * 2. Maintain a deque of candidate halfplanes and a deque of their mutual intersection vertices.
 * 3. Iterate through each halfplane:
 *    - While the last vertex lies outside the new halfplane, pop from the back.
 *    - While the first vertex lies outside the new halfplane, pop from the front.
 *    - Push the new halfplane and its intersection point with the back halfplane.
 * 4. Clean up boundary intersections between front and back of the deque.
 * 5. Compute the resulting convex polygon vertices and area via the Shoelace formula.
 *
 * Time Complexity: O(N log N) dominated by sorting
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <deque>
#include <algorithm>
#include <iomanip>

const double EPS = 1e-9;
const double PI = std::acos(-1.0);

struct Vec2 {
    double x;
    double y;

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(double s) const { return {x * s, y * s}; }
    double cross(const Vec2& o) const { return x * o.y - y * o.x; }
};

struct Halfplane {
    Vec2 p; // Point on the line
    Vec2 d; // Direction vector (valid region is to the left of d)
    double angle;

    Halfplane() : p{0, 0}, d{0, 0}, angle(0) {}
    Halfplane(Vec2 p1, Vec2 p2) : p(p1), d(p2 - p1), angle(std::atan2(d.y, d.x)) {}

    // Check if point q is strictly to the left of this halfplane
    bool contains(const Vec2& q) const {
        return d.cross(q - p) > -EPS;
    }
};

// Line-line intersection
Vec2 computeIntersection(const Halfplane& h1, const Halfplane& h2) {
    double c = h1.d.cross(h2.d);
    Vec2 v = h2.p - h1.p;
    double t = v.cross(h2.d) / c;
    return h1.p + h1.d * t;
}

std::vector<Vec2> intersectHalfplanes(std::vector<Halfplane> planes) {
    // 1. Sort by polar angle
    std::sort(planes.begin(), planes.end(), [](const Halfplane& a, const Halfplane& b) {
        if (std::abs(a.angle - b.angle) > EPS) {
            return a.angle < b.angle;
        }
        // Same angle: keep the leftmost halfplane
        return a.d.cross(b.p - a.p) < 0;
    });

    // Remove halfplanes with the same angle
    std::vector<Halfplane> uniquePlanes;
    for (size_t i = 0; i < planes.size(); ++i) {
        if (i == 0 || std::abs(planes[i].angle - planes[i - 1].angle) > EPS) {
            uniquePlanes.push_back(planes[i]);
        }
    }

    std::deque<Halfplane> dq;
    std::deque<Vec2> pts;

    for (const auto& hp : uniquePlanes) {
        while (pts.size() >= 1 && !hp.contains(pts.back())) {
            pts.pop_back();
            dq.pop_back();
        }
        while (pts.size() >= 1 && !hp.contains(pts.front())) {
            pts.pop_front();
            dq.pop_front();
        }

        if (!dq.empty()) {
            if (std::abs(dq.back().d.cross(hp.d)) < EPS) {
                // Parallel lines with opposite directions
                return {};
            }
            pts.push_back(computeIntersection(dq.back(), hp));
        }
        dq.push_back(hp);
    }

    // Clean up ends
    while (pts.size() >= 1 && !dq.front().contains(pts.back())) {
        pts.pop_back();
        dq.pop_back();
    }

    if (dq.size() < 3) return {};

    // Final closing vertex
    pts.push_back(computeIntersection(dq.back(), dq.front()));

    return {pts.begin(), pts.end()};
}

double polygonArea(const std::vector<Vec2>& poly) {
    double area = 0.0;
    int n = static_cast<int>(poly.size());
    for (int i = 0; i < n; ++i) {
        int next = (i + 1) % n;
        area += poly[i].cross(poly[next]);
    }
    return std::abs(area) / 2.0;
}

int main() {
    std::cout << "=== Halfplane Intersection & Convex Kernel (O(N log N)) ===\n\n";

    // Define bounding box [-10, 10] x [-10, 10] + cuts:
    // Left side of lines is valid.
    std::vector<Halfplane> planes;

    // Bounding Box:
    // Bottom: (-10, -10) -> (10, -10)
    planes.push_back({{-10, -10}, {10, -10}});
    // Right:  (10, -10) -> (10, 10)
    planes.push_back({{10, -10}, {10, 10}});
    // Top:    (10, 10) -> (-10, 10)
    planes.push_back({{10, 10}, {-10, 10}});
    // Left:   (-10, 10) -> (-10, -10)
    planes.push_back({{-10, 10}, {-10, -10}});

    // Diagonal Cuts:
    // Line y >= x - 2  <=>  (0, -2) -> (2, 0)
    planes.push_back({{0, -2}, {2, 0}});
    // Line y <= -x + 6 <=>  (6, 0) -> (0, 6)
    planes.push_back({{6, 0}, {0, 6}});

    std::cout << "Intersecting 6 Halfplanes (4 bounding box edges + 2 diagonal cuts):\n";
    auto poly = intersectHalfplanes(planes);

    if (poly.empty()) {
        std::cout << "Intersection region is empty!\n";
    } else {
        std::cout << "Intersection Region: Convex Polygon with " << poly.size() << " vertices:\n";
        for (size_t i = 0; i < poly.size(); ++i) {
            std::cout << "  Vertex " << i << ": ("
                      << std::fixed << std::setprecision(3) << std::setw(7) << poly[i].x << ", "
                      << std::setw(7) << poly[i].y << ")\n";
        }

        double area = polygonArea(poly);
        std::cout << "\nFeasible Region Area: " << std::fixed << std::setprecision(4) << area << '\n';
    }

    return 0;
}
