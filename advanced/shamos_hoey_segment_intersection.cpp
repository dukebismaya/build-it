/**
 * @file shamos_hoey_segment_intersection.cpp
 * @brief Shamos-Hoey sweep-line algorithm for detecting segment intersections in C++17.
 *
 * Overview (Michael I. Shamos & Dan Hoey, 1976):
 *   Determines whether any pair of segments among N 2D line segments intersect.
 *   While brute-force pairwise checking requires O(N^2) time, the Shamos-Hoey
 *   sweep-line algorithm solves the problem in O(N log N) time.
 *
 * Algorithmic Principle:
 *   - A vertical line sweeps from left to right across the 2N segment endpoints.
 *   - Maintain a dynamic status structure (balanced BST / std::set) of active segments
 *     intersecting the current sweep line, ordered by their y-coordinates at the sweep position.
 *   - Key Theorem: If any two segments intersect, they must become vertical neighbors
 *     in the status structure immediately prior to the intersection point.
 *   - Events:
 *       1. Left Endpoint (INSERT segment s):
 *          Check if s intersects its predecessor or successor in the status structure.
 *       2. Right Endpoint (DELETE segment s):
 *          Check if the predecessor and successor of s intersect each other.
 *
 * Complexity:
 *   - Time: O(N log N)
 *   - Space: O(N)
 */

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace geometry {

struct Point {
    double x;
    double y;
};

// Global sweep line x-coordinate used by Segment comparator
inline double sweepX = 0.0;

struct Segment {
    int id;
    Point left;
    Point right;

    Segment(int sId, Point p1, Point p2) : id(sId) {
        if (p1.x < p2.x || (std::abs(p1.x - p2.x) < 1e-9 && p1.y < p2.y)) {
            left = p1;
            right = p2;
        } else {
            left = p2;
            right = p1;
        }
    }

    /**
     * @brief Computes y-coordinate of the segment at current sweepX.
     */
    double getY(double x) const {
        if (std::abs(left.x - right.x) < 1e-9) {
            return left.y;
        }
        return left.y + (right.y - left.y) * (x - left.x) / (right.x - left.x);
    }
};

struct SegmentComparator {
    bool operator()(const Segment& a, const Segment& b) const {
        if (a.id == b.id) return false;
        double ya = a.getY(sweepX);
        double yb = b.getY(sweepX);
        if (std::abs(ya - yb) > 1e-9) {
            return ya < yb;
        }
        return a.id < b.id;
    }
};

enum EventType { EVENT_LEFT, EVENT_RIGHT };

struct Event {
    double x;
    double y;
    EventType type;
    Segment seg;

    bool operator<(const Event& o) const {
        if (std::abs(x - o.x) > 1e-9) return x < o.x;
        if (type != o.type) return type < o.type; // Left endpoints processed before right
        return y < o.y;
    }
};

double crossProduct(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool onSegment(Point p, Point q, Point r) {
    return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
}

bool doIntersect(const Segment& s1, const Segment& s2) {
    Point p1 = s1.left, q1 = s1.right;
    Point p2 = s2.left, q2 = s2.right;

    double d1 = crossProduct(p2, q2, p1);
    double d2 = crossProduct(p2, q2, q1);
    double d3 = crossProduct(p1, q1, p2);
    double d4 = crossProduct(p1, q1, q2);

    if (((d1 > 1e-9 && d2 < -1e-9) || (d1 < -1e-9 && d2 > 1e-9)) &&
        ((d3 > 1e-9 && d4 < -1e-9) || (d3 < -1e-9 && d4 > 1e-9))) {
        return true;
    }

    if (std::abs(d1) < 1e-9 && onSegment(p2, p1, q2)) return true;
    if (std::abs(d2) < 1e-9 && onSegment(p2, q1, q2)) return true;
    if (std::abs(d3) < 1e-9 && onSegment(p1, p2, q1)) return true;
    if (std::abs(d4) < 1e-9 && onSegment(p1, q2, q1)) return true;

    return false;
}

class ShamosHoey {
public:
    struct Result {
        bool hasIntersection;
        int segA;
        int segB;
    };

    static Result checkIntersection(const std::vector<Segment>& segments) {
        std::vector<Event> events;
        events.reserve(segments.size() * 2);

        for (const auto& s : segments) {
            events.push_back({s.left.x, s.left.y, EVENT_LEFT, s});
            events.push_back({s.right.x, s.right.y, EVENT_RIGHT, s});
        }

        std::sort(events.begin(), events.end());

        std::set<Segment, SegmentComparator> status;

        for (const auto& ev : events) {
            sweepX = ev.x;

            if (ev.type == EVENT_LEFT) {
                auto it = status.insert(ev.seg).first;

                // Check with predecessor
                if (it != status.begin()) {
                    auto pred = std::prev(it);
                    if (doIntersect(*it, *pred)) {
                        return {true, it->id, pred->id};
                    }
                }

                // Check with successor
                auto succ = std::next(it);
                if (succ != status.end()) {
                    if (doIntersect(*it, *succ)) {
                        return {true, it->id, succ->id};
                    }
                }
            } else {
                auto it = status.find(ev.seg);
                if (it != status.end()) {
                    auto pred = (it != status.begin()) ? std::prev(it) : status.end();
                    auto succ = std::next(it);

                    if (pred != status.end() && succ != status.end()) {
                        if (doIntersect(*pred, *succ)) {
                            return {true, pred->id, succ->id};
                        }
                    }
                    status.erase(it);
                }
            }
        }

        return {false, -1, -1};
    }
};

} // namespace geometry

int main() {
    std::cout << "========================================\n";
    std::cout << "   SHAMOS-HOEY SWEEP-LINE ALGORITHM     \n";
    std::cout << " (Segment Intersection Detection O(NlogN))\n";
    std::cout << "========================================\n\n";

    // Test Case 1: Intersecting Segments
    std::vector<geometry::Segment> test1 = {
        {1, {1.0, 5.0}, {4.0, 5.0}},
        {2, {2.0, 3.0}, {5.0, 7.0}}, // Intersects segment 1 at (3.5, 5.0)
        {3, {6.0, 1.0}, {8.0, 3.0}}
    };

    std::cout << "Dataset 1 (Contains intersecting segments):\n";
    for (const auto& s : test1) {
        std::cout << "  Segment " << s.id << ": (" << s.left.x << "," << s.left.y
                  << ") -> (" << s.right.x << "," << s.right.y << ")\n";
    }

    auto res1 = geometry::ShamosHoey::checkIntersection(test1);
    std::cout << "Result 1: " << (res1.hasIntersection ? "INTERSECTION DETECTED" : "NO INTERSECTION") << "\n";
    if (res1.hasIntersection) {
        std::cout << "  Intersecting pair: Segment " << res1.segA << " and Segment " << res1.segB << "\n\n";
    }

    // Test Case 2: Disjoint Segments
    std::vector<geometry::Segment> test2 = {
        {1, {1.0, 1.0}, {3.0, 3.0}},
        {2, {1.0, 4.0}, {3.0, 6.0}},
        {3, {4.0, 1.0}, {6.0, 2.0}},
        {4, {4.0, 5.0}, {7.0, 6.0}}
    };

    std::cout << "Dataset 2 (Completely disjoint segments):\n";
    for (const auto& s : test2) {
        std::cout << "  Segment " << s.id << ": (" << s.left.x << "," << s.left.y
                  << ") -> (" << s.right.x << "," << s.right.y << ")\n";
    }

    auto res2 = geometry::ShamosHoey::checkIntersection(test2);
    std::cout << "Result 2: " << (res2.hasIntersection ? "INTERSECTION DETECTED" : "NO INTERSECTION (Clean)") << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
