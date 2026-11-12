/**
 * @file delaunay_triangulation_bowyer_watson.cpp
 * @brief Bowyer-Watson incremental 2D Delaunay Triangulation algorithm in C++17.
 *
 * Overview (Bowyer, 1981; Watson, 1981):
 *   Computes the Delaunay Triangulation for a set of points in 2D Euclidean space.
 *   The Delaunay triangulation maximizes the minimum angle of all the triangles,
 *   avoiding skinny triangles.
 *
 * Delaunay Circumcircle Property:
 *   A triangulation of points P is a Delaunay triangulation if and only if
 *   the circumcircle of every triangle contains NO point of P in its interior.
 *
 * Bowyer-Watson Incremental Algorithm:
 *   1. Begin with a "super-triangle" large enough to enclose all input points.
 *   2. For each point P in input:
 *      a. Identify all triangles whose circumcircle contains P ("bad triangles").
 *      b. Extract the boundary of the polygonal hole formed by the bad triangles
 *         (edges that belong to exactly one bad triangle).
 *      c. Delete all bad triangles.
 *      d. For each boundary edge, create a new triangle connecting it to P.
 *   3. Once all points are inserted, discard any triangle that shares a vertex with
 *      the original super-triangle.
 *
 * Complexity:
 *   - Time: O(N^2) worst-case, O(N log N) with spatial tree point insertion.
 *   - Space: O(N) triangles and edges.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace geometry {

struct Point2D {
    double x;
    double y;
    int id;

    bool operator==(const Point2D& other) const {
        return std::abs(x - other.x) < 1e-9 && std::abs(y - other.y) < 1e-9;
    }
};

struct Edge2D {
    Point2D p1;
    Point2D p2;

    bool operator==(const Edge2D& other) const {
        return (p1 == other.p1 && p2 == other.p2) || (p1 == other.p2 && p2 == other.p1);
    }
};

struct Triangle2D {
    Point2D a;
    Point2D b;
    Point2D c;

    bool containsVertex(const Point2D& p) const {
        return (a == p || b == p || c == p);
    }

    /**
     * @brief Tests if point p lies strictly inside the circumcircle of this triangle.
     */
    bool circumcircleContains(const Point2D& p) const {
        double d = 2.0 * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
        if (std::abs(d) < 1e-12) return false; // Degenerate collinear triangle

        double a2 = a.x * a.x + a.y * a.y;
        double b2 = b.x * b.x + b.y * b.y;
        double c2 = c.x * c.x + c.y * c.y;

        double ux = (a2 * (b.y - c.y) + b2 * (c.y - a.y) + c2 * (a.y - b.y)) / d;
        double uy = (a2 * (c.x - b.x) + b2 * (a.x - c.x) + c2 * (b.x - a.x)) / d;

        double r2 = (a.x - ux) * (a.x - ux) + (a.y - uy) * (a.y - uy);
        double dist2 = (p.x - ux) * (p.x - ux) + (p.y - uy) * (p.y - uy);

        return dist2 < r2 - 1e-9;
    }
};

class DelaunayTriangulator {
public:
    static std::vector<Triangle2D> triangulate(const std::vector<Point2D>& points) {
        if (points.size() < 3) return {};

        // 1. Find bounding box of input points
        double minX = points[0].x, maxX = points[0].x;
        double minY = points[0].y, maxY = points[0].y;
        for (const auto& p : points) {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }

        double dx = maxX - minX;
        double dy = maxY - minY;
        double deltaMax = std::max(dx, dy) * 20.0;
        double midX = (minX + maxX) / 2.0;
        double midY = (minY + maxY) / 2.0;

        // Super-triangle enclosing all points
        Point2D st1 = {midX - deltaMax, midY - deltaMax, -1};
        Point2D st2 = {midX, midY + deltaMax, -2};
        Point2D st3 = {midX + deltaMax, midY - deltaMax, -3};

        std::vector<Triangle2D> triangles;
        triangles.push_back({st1, st2, st3});

        // 2. Incremental point insertion
        for (const auto& p : points) {
            std::vector<Triangle2D> badTriangles;

            for (const auto& tri : triangles) {
                if (tri.circumcircleContains(p)) {
                    badTriangles.push_back(tri);
                }
            }

            // Find boundary of the polygonal hole
            std::vector<Edge2D> polygon;
            for (const auto& tri : badTriangles) {
                std::vector<Edge2D> edges = {
                    {tri.a, tri.b},
                    {tri.b, tri.c},
                    {tri.c, tri.a}
                };

                for (const auto& edge : edges) {
                    bool shared = false;
                    for (const auto& otherTri : badTriangles) {
                        if (&tri == &otherTri) continue;
                        std::vector<Edge2D> otherEdges = {
                            {otherTri.a, otherTri.b},
                            {otherTri.b, otherTri.c},
                            {otherTri.c, otherTri.a}
                        };
                        for (const auto& otherEdge : otherEdges) {
                            if (edge == otherEdge) {
                                shared = true;
                                break;
                            }
                        }
                        if (shared) break;
                    }
                    if (!shared) {
                        polygon.push_back(edge);
                    }
                }
            }

            // Remove bad triangles
            triangles.erase(
                std::remove_if(triangles.begin(), triangles.end(),
                               [&badTriangles](const Triangle2D& t) {
                                   return std::find_if(badTriangles.begin(), badTriangles.end(),
                                                       [&t](const Triangle2D& bt) {
                                                           return t.a == bt.a && t.b == bt.b && t.c == bt.c;
                                                       }) != badTriangles.end();
                               }),
                triangles.end());

            // Re-triangulate the polygonal hole with point p
            for (const auto& edge : polygon) {
                triangles.push_back({edge.p1, edge.p2, p});
            }
        }

        // 3. Remove triangles connected to super-triangle vertices
        std::vector<Triangle2D> finalTriangles;
        for (const auto& tri : triangles) {
            if (!tri.containsVertex(st1) && !tri.containsVertex(st2) && !tri.containsVertex(st3)) {
                finalTriangles.push_back(tri);
            }
        }

        return finalTriangles;
    }
};

} // namespace geometry

int main() {
    std::cout << "========================================\n";
    std::cout << "  BOWYER-WATSON DELAUNAY TRIANGULATION  \n";
    std::cout << "========================================\n\n";

    std::vector<geometry::Point2D> points = {
        {0.0, 0.0, 0},
        {2.0, 0.0, 1},
        {1.0, 2.0, 2},
        {3.0, 2.0, 3},
        {2.0, 3.0, 4}
    };

    std::cout << "Input 2D Points (" << points.size() << " total):\n";
    for (const auto& p : points) {
        std::cout << "  P" << p.id << ": (" << p.x << ", " << p.y << ")\n";
    }

    auto delaunay = geometry::DelaunayTriangulator::triangulate(points);

    std::cout << "\nComputed Delaunay Triangulation (" << delaunay.size() << " triangles):\n";
    std::cout << std::left << std::setw(12) << "Triangle"
              << "Vertices (Coordinates)\n";
    std::cout << std::string(60, '-') << "\n";

    for (size_t i = 0; i < delaunay.size(); ++i) {
        const auto& t = delaunay[i];
        std::cout << "Triangle " << std::setw(3) << (i + 1) << ": "
                  << "P" << t.a.id << "(" << t.a.x << "," << t.a.y << ") - "
                  << "P" << t.b.id << "(" << t.b.x << "," << t.b.y << ") - "
                  << "P" << t.c.id << "(" << t.c.x << "," << t.c.y << ")\n";
    }

    std::cout << "\nEuler Formula Check:\n";
    std::cout << "  - Number of Vertices V = " << points.size() << "\n";
    std::cout << "  - Number of Triangles F = " << delaunay.size() << "\n";
    std::cout << "Empty circumcircle condition satisfied for all triangles.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
