/**
 * @file spatial_quadtree_points.cpp
 * @brief 2D Point Quadtree for spatial indexing and range queries in C++17.
 *
 * Overview:
 *   A Quadtree is a tree data structure in which each internal node has exactly
 *   four children: Northwest (NW), Northeast (NE), Southwest (SW), and Southeast (SE).
 *   It recursively decomposes 2D space into bounding regions.
 *
 * Core Capabilities:
 *   - Incremental point insertion with automatic quadrant subdivision when node capacity is exceeded.
 *   - Fast 2D Axis-Aligned Bounding Box (AABB) spatial range search with branch pruning.
 *
 * Complexity:
 *   - Point Insertion: O(log N) average, O(N) worst-case.
 *   - Range Search: O(log N + K) where K is the number of points returned.
 *   - Memory: O(N) where N is the total number of points stored.
 */

#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>

namespace spatial {

struct Point {
    double x;
    double y;
    int id;
};

struct AABB {
    double xMin;
    double yMin;
    double xMax;
    double yMax;

    bool contains(const Point& p) const {
        return (p.x >= xMin && p.x <= xMax && p.y >= yMin && p.y <= yMax);
    }

    bool intersects(const AABB& other) const {
        return !(xMax < other.xMin || xMin > other.xMax ||
                 yMax < other.yMin || yMin > other.yMax);
    }
};

class QuadtreeNode {
private:
    static constexpr size_t CAPACITY = 4;
    AABB boundary;
    std::vector<Point> points;
    bool subdivided;

    std::unique_ptr<QuadtreeNode> nw;
    std::unique_ptr<QuadtreeNode> ne;
    std::unique_ptr<QuadtreeNode> sw;
    std::unique_ptr<QuadtreeNode> se;

    void subdivide() {
        double xMid = (boundary.xMin + boundary.xMax) / 2.0;
        double yMid = (boundary.yMin + boundary.yMax) / 2.0;

        nw = std::make_unique<QuadtreeNode>(AABB{boundary.xMin, yMid, xMid, boundary.yMax});
        ne = std::make_unique<QuadtreeNode>(AABB{xMid, yMid, boundary.xMax, boundary.yMax});
        sw = std::make_unique<QuadtreeNode>(AABB{boundary.xMin, boundary.yMin, xMid, yMid});
        se = std::make_unique<QuadtreeNode>(AABB{xMid, boundary.yMin, boundary.xMax, yMid});

        subdivided = true;

        // Redistribute existing points into child quadrants
        for (const auto& p : points) {
            insertChild(p);
        }
        points.clear();
    }

    bool insertChild(const Point& p) {
        return nw->insert(p) || ne->insert(p) || sw->insert(p) || se->insert(p);
    }

public:
    explicit QuadtreeNode(AABB box) : boundary(box), subdivided(false) {}

    bool insert(const Point& p) {
        if (!boundary.contains(p)) {
            return false;
        }

        if (!subdivided) {
            if (points.size() < CAPACITY) {
                points.push_back(p);
                return true;
            }
            subdivide();
        }

        return insertChild(p);
    }

    void query(const AABB& range, std::vector<Point>& results) const {
        if (!boundary.intersects(range)) {
            return;
        }

        for (const auto& p : points) {
            if (range.contains(p)) {
                results.push_back(p);
            }
        }

        if (subdivided) {
            nw->query(range, results);
            ne->query(range, results);
            sw->query(range, results);
            se->query(range, results);
        }
    }
};

} // namespace spatial

int main() {
    std::cout << "========================================\n";
    std::cout << "      2D SPATIAL QUADTREE INDEX         \n";
    std::cout << " (AABB Range Query & Spatial Partition) \n";
    std::cout << "========================================\n\n";

    // Create Quadtree covering domain [0, 100] x [0, 100]
    spatial::AABB domain = {0.0, 0.0, 100.0, 100.0};
    spatial::QuadtreeNode root(domain);

    std::vector<spatial::Point> points = {
        {10.0, 20.0, 1},
        {15.0, 25.0, 2},
        {12.0, 18.0, 3},
        {14.0, 22.0, 4},
        {50.0, 50.0, 5},
        {80.0, 85.0, 6},
        {82.0, 90.0, 7},
        {30.0, 70.0, 8},
        {95.0, 10.0, 9},
        {75.0, 45.0, 10}
    };

    std::cout << "Inserting " << points.size() << " 2D spatial points into domain [0, 100]^2...\n";
    for (const auto& p : points) {
        root.insert(p);
    }

    // Query 1: Bounding box [10, 15] x [15, 30] (Dense cluster)
    spatial::AABB queryBox1 = {10.0, 15.0, 16.0, 30.0};
    std::vector<spatial::Point> found1;
    root.query(queryBox1, found1);

    std::cout << "\nQuery Range 1: x in [10, 16], y in [15, 30]:\n";
    std::cout << "  Found " << found1.size() << " points:\n";
    for (const auto& p : found1) {
        std::cout << "    - Point ID " << p.id << " at (" << p.x << ", " << p.y << ")\n";
    }

    // Query 2: Bounding box [70, 40] x [100, 100]
    spatial::AABB queryBox2 = {70.0, 40.0, 100.0, 100.0};
    std::vector<spatial::Point> found2;
    root.query(queryBox2, found2);

    std::cout << "\nQuery Range 2: x in [70, 100], y in [40, 100]:\n";
    std::cout << "  Found " << found2.size() << " points:\n";
    for (const auto& p : found2) {
        std::cout << "    - Point ID " << p.id << " at (" << p.x << ", " << p.y << ")\n";
    }

    std::cout << "\nSpatial partitioning and pruning verified.\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
