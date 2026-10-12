/*
 * Problem Description:
 * Graham Scan Algorithm for Finding the 2D Convex Hull.
 * Given a set of n points in the 2D plane, compute the smallest convex polygon that
 * encloses all points.
 * The algorithm selects the point with lowest y-coordinate (and lowest x in case of tie) as pivot,
 * sorts the remaining points by polar angle with respect to the pivot, and processes points sequentially
 * using a stack, discarding points that create a clockwise (non-left) turn.
 * Time Complexity: O(N log N) dominated by sorting.
 * Space Complexity: O(N) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <algorithm>

struct Point {
    long long x, y;
};

// Returns squared Euclidean distance between two points
long long distSq(Point p1, Point p2) {
    return (p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

// Determines orientation of ordered triplet (p, q, r)
// Returns:
//  0 : p, q and r are collinear
//  1 : Clockwise turn
//  2 : Counterclockwise turn
int orientation(Point p, Point q, Point r) {
    long long val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0) ? 1 : 2;
}

Point p0; // Global pivot point for comparator

bool comparePoints(Point p1, Point p2) {
    int o = orientation(p0, p1, p2);
    if (o == 0) {
        return distSq(p0, p1) < distSq(p0, p2);
    }
    return (o == 2);
}

std::vector<Point> convexHull(std::vector<Point>& points) {
    int n = points.size();
    if (n < 3) return points;

    // Find the bottom-most point (and left-most in case of tie)
    int ymin = 0;
    for (int i = 1; i < n; ++i) {
        if ((points[i].y < points[ymin].y) ||
            (points[i].y == points[ymin].y && points[i].x < points[ymin].x)) {
            ymin = i;
        }
    }

    std::swap(points[0], points[ymin]);
    p0 = points[0];

    std::sort(points.begin() + 1, points.end(), comparePoints);

    // Remove collinear points keeping only the farthest
    int m = 1;
    for (int i = 1; i < n; ++i) {
        while (i < n - 1 && orientation(p0, points[i], points[i + 1]) == 0) {
            i++;
        }
        points[m++] = points[i];
    }

    if (m < 3) return {};

    std::vector<Point> hull;
    hull.push_back(points[0]);
    hull.push_back(points[1]);
    hull.push_back(points[2]);

    for (int i = 3; i < m; ++i) {
        while (hull.size() >= 2 && orientation(hull[hull.size() - 2], hull.back(), points[i]) != 2) {
            hull.pop_back();
        }
        hull.push_back(points[i]);
    }

    return hull;
}

int main() {
    std::vector<Point> points = {
        {0, 3}, {1, 1}, {2, 2}, {4, 4},
        {0, 0}, {1, 2}, {3, 1}, {3, 3}
    };

    std::vector<Point> hull = convexHull(points);

    std::cout << "Convex Hull Vertices (Graham Scan):\n";
    for (const auto& pt : hull) {
        std::cout << "(" << pt.x << ", " << pt.y << ")\n";
    }

    return 0;
}
