/*
 * ============================================================
 *    Polygon Ear-Clipping Triangulation & Funnel Shortest Path
 * ============================================================
 *
 * Description:
 *   This program implements two foundational algorithms used in
 *   game engine Navigation Meshes (Recast/Detour) and computational
 *   geometry:
 *
 *   1. Ear-Clipping Triangulation of a Simple Polygon:
 *      By Meisters' Two-Ears Theorem (1975), every simple polygon
 *      with N >= 4 vertices has at least two non-overlapping "ears"
 *      (triangles (v_{i-1}, v_i, v_{i+1}) whose interior angle at
 *      v_i is strictly convex and that contain no other reflex
 *      polygon vertex). Repeatedly clipping an ear decomposes any
 *      simple polygon into N - 2 triangles in O(N^2) time.
 *
 *   2. Simple Stupid Funnel Algorithm (Chazelle 1982 / Lee & Preparata):
 *      Given a sequence of portals (shared interior edges of the
 *      channel of triangles connecting start S to goal G), the
 *      Funnel Algorithm maintains an apex and left/right concave
 *      chains in a double-ended queue, tightening the funnel and
 *      emitting a corner waypoint whenever one side crosses over
 *      the other, computing the exact Euclidean shortest path
 *      ("string pulling") through the channel in O(N) linear time!
 *
 * Complexity:
 *   Ear Clipping:     O(N^2) time, produces N - 2 triangles
 *   Funnel Pathing:   O(K) time for a channel of K portals
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace navmesh_funnel {

    struct Vec2 {
        double x, y;
        Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
        double dist(const Vec2& o) const { return std::hypot(x - o.x, y - o.y); }
    };

    // Signed 2x triangle area: > 0 if (a, b, c) is counter-clockwise
    double tri_area2(const Vec2& a, const Vec2& b, const Vec2& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }

    bool point_in_triangle(const Vec2& p, const Vec2& a, const Vec2& b, const Vec2& c) {
        double c1 = tri_area2(a, b, p);
        double c2 = tri_area2(b, c, p);
        double c3 = tri_area2(c, a, p);
        return c1 >= -1e-11 && c2 >= -1e-11 && c3 >= -1e-11;
    }

    struct Triangle { int a, b, c; };

    double polygon_signed_area(const std::vector<Vec2>& poly) {
        double a = 0.0;
        for (size_t i = 0; i < poly.size(); ++i) {
            const auto& p = poly[i];
            const auto& q = poly[(i + 1) % poly.size()];
            a += p.x * q.y - q.x * p.y;
        }
        return 0.5 * a;
    }

    std::vector<Triangle> ear_clip_triangulate(const std::vector<Vec2>& poly) {
        int n = static_cast<int>(poly.size());
        std::vector<int> idx(n);
        for (int i = 0; i < n; ++i) idx[i] = i;
        if (polygon_signed_area(poly) < 0)
            std::reverse(idx.begin(), idx.end());

        std::vector<Triangle> tris;
        while (idx.size() > 3) {
            int m = static_cast<int>(idx.size());
            bool clipped = false;
            for (int i = 0; i < m; ++i) {
                int ip = idx[(i + m - 1) % m];
                int ic = idx[i];
                int in = idx[(i + 1) % m];
                if (tri_area2(poly[ip], poly[ic], poly[in]) <= 1e-11)
                    continue; // reflex or collinear

                bool empty = true;
                for (int j = 0; j < m; ++j) {
                    int iv = idx[j];
                    if (iv == ip || iv == ic || iv == in) continue;
                    if (point_in_triangle(poly[iv], poly[ip], poly[ic], poly[in])) {
                        empty = false;
                        break;
                    }
                }
                if (empty) {
                    tris.push_back({ip, ic, in});
                    idx.erase(idx.begin() + i);
                    clipped = true;
                    break;
                }
            }
            if (!clipped) break;
        }
        if (idx.size() == 3) tris.push_back({idx[0], idx[1], idx[2]});
        return tris;
    }

    struct Portal {
        Vec2 left, right; // oriented so moving forward keeps left on the left and right on the right
    };

    std::vector<Vec2> string_pull_funnel(const Vec2& start, const Vec2& goal,
                                         std::vector<Portal> portals) {
        portals.push_back({goal, goal});
        std::vector<Vec2> path = {start};

        Vec2 apex = start, left = start, right = start;
        int apex_idx = 0, left_idx = 0, right_idx = 0;

        for (int i = 0; i < static_cast<int>(portals.size()); ++i) {
            const Vec2& p_left  = portals[i].left;
            const Vec2& p_right = portals[i].right;

            // Update right side of funnel
            if (tri_area2(apex, right, p_right) >= -1e-12) {
                if ((apex.x == right.x && apex.y == right.y) ||
                    tri_area2(apex, left, p_right) < 1e-12) {
                    right = p_right;
                    right_idx = i;
                } else {
                    // Right crossed over left -> left becomes new apex!
                    path.push_back(left);
                    apex = left;
                    apex_idx = left_idx;
                    left = apex;
                    right = apex;
                    left_idx = right_idx = apex_idx;
                    i = apex_idx;
                    continue;
                }
            }

            // Update left side of funnel
            if (tri_area2(apex, left, p_left) <= 1e-12) {
                if ((apex.x == left.x && apex.y == left.y) ||
                    tri_area2(apex, right, p_left) > -1e-12) {
                    left = p_left;
                    left_idx = i;
                } else {
                    // Left crossed over right -> right becomes new apex!
                    path.push_back(right);
                    apex = right;
                    apex_idx = right_idx;
                    left = apex;
                    right = apex;
                    left_idx = right_idx = apex_idx;
                    i = apex_idx;
                    continue;
                }
            }
        }
        path.push_back(goal);
        return path;
    }

} // namespace navmesh_funnel

int main() {
    using namespace navmesh_funnel;
    std::cout << "============================================================\n";
    std::cout << "    Polygon Ear-Clipping Triangulation & Funnel Shortest Path\n";
    std::cout << "============================================================\n\n";

    // S-shaped non-convex corridor polygon (12 vertices, non-self-intersecting)
    std::vector<Vec2> poly = {
        {0, 0}, {6, 0}, {6, 4}, {8, 4}, {8, 0}, {10, 0},
        {10, 6}, {4, 6}, {4, 2}, {2, 2}, {2, 5}, {0, 5}
    };

    double poly_area = std::fabs(polygon_signed_area(poly));
    auto tris = ear_clip_triangulate(poly);
    double sum_tri_area = 0.0;
    for (const auto& t : tris)
        sum_tri_area += 0.5 * std::fabs(tri_area2(poly[t.a], poly[t.b], poly[t.c]));

    std::cout << "Triangulated 12-vertex non-convex polygon into " << tris.size()
              << " triangles (expected N - 2 = 10).\n";
    std::cout << "Polygon area = " << std::fixed << std::setprecision(2) << poly_area
              << ", sum of triangle areas = " << sum_tri_area << "\n\n";

    // Funnel test through the S-corridor portals from (1, 4) to (9, 1)
    // Exact taut-string path bends around reflex corners (2, 2), (6, 4), and (8, 4)
    Vec2 start{1.0, 4.0}, goal{9.0, 1.0};
    std::vector<Portal> portals = {
        {{2.0, 2.0}, {0.0, 0.0}},
        {{4.0, 2.0}, {6.0, 0.0}},
        {{4.0, 6.0}, {6.0, 4.0}},
        {{10.0, 6.0}, {8.0, 4.0}}
    };
    auto path = string_pull_funnel(start, goal, portals);

    double total_len = 0.0;
    std::cout << "Funnel shortest path waypoints:\n  ";
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) {
            std::cout << " -> ";
            total_len += path[i - 1].dist(path[i]);
        }
        std::cout << "(" << path[i].x << ", " << path[i].y << ")";
    }
    double exact_len = start.dist({2.0, 2.0}) + Vec2{2.0, 2.0}.dist({4.0, 2.0})
                     + Vec2{4.0, 2.0}.dist({6.0, 4.0}) + Vec2{6.0, 4.0}.dist({8.0, 4.0})
                     + Vec2{8.0, 4.0}.dist(goal);
    std::cout << "\nFunnel path length = " << std::setprecision(6) << total_len
              << " (analytic taut-string length = " << exact_len << ")\n";

    bool ok = (tris.size() == poly.size() - 2) &&
              (std::fabs(poly_area - sum_tri_area) < 1e-9) &&
              (std::fabs(total_len - exact_len) < 1e-9);
    std::cout << "\nTriangulation area conservation & taut-string funnel optimality: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
