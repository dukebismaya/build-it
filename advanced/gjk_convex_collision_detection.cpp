/*
 * ============================================================
 *     GJK (Gilbert-Johnson-Keerthi) Convex Collision & Distance
 * ============================================================
 *
 * Description:
 *   The Gilbert-Johnson-Keerthi (GJK, 1988) algorithm determines
 *   whether two convex shapes A and B intersect (and computes the
 *   Euclidean distance between them when disjoint) by working on
 *   their Minkowski Difference:
 *     A (-) B = { a - b : a in A, b in B }
 *   Two shapes overlap iff the origin (0, 0) lies inside A (-) B.
 *
 *   Instead of constructing A (-) B explicitly, GJK queries only
 *   the support mapping:
 *     support(A (-) B, d) = support(A, d) - support(B, -d)
 *   and iteratively evolves a simplex (1, 2, or 3 vertices in 2D)
 *   inside A (-) B toward the origin using Voronoi region tests
 *   and triple-product normals.
 *
 * Complexity:
 *   Typically 3-6 simplex iterations in 2D/3D (effectively O(1)
 *   iterations times support query cost).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <iomanip>

namespace gjk_2d {

    struct Vec2 {
        double x, y;
        Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
        Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
        Vec2 operator-() const { return {-x, -y}; }
        Vec2 operator*(double s) const { return {x * s, y * s}; }
        double dot(const Vec2& o) const { return x * o.x + y * o.y; }
        double cross(const Vec2& o) const { return x * o.y - y * o.x; }
        double norm_sq() const { return x * x + y * y; }
    };

    // (a x b) x c = b * (a . c) - a * (b . c)
    Vec2 triple_product(const Vec2& a, const Vec2& b, const Vec2& c) {
        return b * a.dot(c) - a * b.dot(c);
    }

    using Polygon = std::vector<Vec2>;

    Vec2 support_poly(const Polygon& poly, const Vec2& d) {
        size_t best = 0;
        double best_dot = poly[0].dot(d);
        for (size_t i = 1; i < poly.size(); ++i) {
            double v = poly[i].dot(d);
            if (v > best_dot) { best_dot = v; best = i; }
        }
        return poly[best];
    }

    Vec2 minkowski_support(const Polygon& A, const Polygon& B, const Vec2& d) {
        return support_poly(A, d) - support_poly(B, -d);
    }

    //Closest point on segment PQ to the origin (0, 0)
    Vec2 closest_on_segment_to_origin(const Vec2& p, const Vec2& q) {
        Vec2 ab = q - p;
        double denom = ab.norm_sq();
        if (denom < 1e-18) return p;
        double t = std::clamp(-p.dot(ab) / denom, 0.0, 1.0);
        return p + ab * t;
    }

    struct GJKResult {
        bool intersecting;
        double distance;
    };

    // Point-in-triangle (including boundary) check for origin (0,0)
    bool origin_in_triangle(const Vec2& a, const Vec2& b, const Vec2& c) {
        double c1 = (b - a).cross(-a);
        double c2 = (c - b).cross(-b);
        double c3 = (a - c).cross(-c);
        bool has_neg = (c1 < -1e-12) || (c2 < -1e-12) || (c3 < -1e-12);
        bool has_pos = (c1 >  1e-12) || (c2 >  1e-12) || (c3 >  1e-12);
        return !(has_neg && has_pos);
    }

    // Reduce simplex S (size 1, 2, or 3) to the minimal feature closest to the origin
    // and return the closest point v on conv(S) to (0, 0).
    Vec2 reduce_simplex(std::vector<Vec2>& S) {
        if (S.size() == 1) return S[0];
        if (S.size() == 2) {
            Vec2 p = S[0], q = S[1], ab = q - p;
            double denom = ab.norm_sq();
            if (denom < 1e-18) { S = {p}; return p; }
            double t = -p.dot(ab) / denom;
            if (t <= 0.0) { S = {p}; return p; }
            if (t >= 1.0) { S = {q}; return q; }
            return p + ab * t;
        }
        // S.size() == 3
        if (origin_in_triangle(S[0], S[1], S[2])) return {0.0, 0.0};
        Vec2 best_v{1e150, 1e150};
        std::vector<Vec2> best_sub;
        for (int i = 0; i < 3; ++i) {
            std::vector<Vec2> edge = {S[i], S[(i + 1) % 3]};
            Vec2 cand = reduce_simplex(edge);
            if (cand.norm_sq() < best_v.norm_sq()) {
                best_v = cand;
                best_sub = edge;
            }
        }
        S = best_sub;
        return best_v;
    }

    GJKResult gjk_intersect_and_distance(const Polygon& A, const Polygon& B) {
        std::vector<Vec2> simplex = {minkowski_support(A, B, {1.0, 0.5})};
        Vec2 v = simplex[0];

        for (int iter = 0; iter < 60; ++iter) {
            v = reduce_simplex(simplex);
            if (v.norm_sq() < 1e-16) return {true, 0.0};

            Vec2 d = -v;
            Vec2 a = minkowski_support(A, B, d);
            // If new support point makes no progress toward origin past v, v is optimal
            if (d.dot(a - v) <= 1e-11)
                return {false, std::sqrt(v.norm_sq())};

            simplex.push_back(a);
        }
        return {v.norm_sq() < 1e-12, std::sqrt(v.norm_sq())};
    }

    // Brute-force Separating Axis Theorem (SAT) + segment-to-segment distance for verification
    GJKResult sat_reference(const Polygon& A, const Polygon& B) {
        auto check_axes = [](const Polygon& P, const Polygon& Q) {
            for (size_t i = 0; i < P.size(); ++i) {
                Vec2 e = P[(i + 1) % P.size()] - P[i];
                Vec2 axis{-e.y, e.x};
                double min_p = 1e300, max_p = -1e300, min_q = 1e300, max_q = -1e300;
                for (const auto& v : P) { double p = v.dot(axis); min_p = std::min(min_p, p); max_p = std::max(max_p, p); }
                for (const auto& v : Q) { double p = v.dot(axis); min_q = std::min(min_q, p); max_q = std::max(max_q, p); }
                if (max_p < min_q - 1e-11 || max_q < min_p - 1e-11) return false;
            }
            return true;
        };
        if (check_axes(A, B) && check_axes(B, A)) return {true, 0.0};

        double best = 1e300;
        auto pt_seg = [](const Vec2& p, const Vec2& a, const Vec2& b) {
            Vec2 c = closest_on_segment_to_origin(a - p, b - p);
            return std::sqrt(c.norm_sq());
        };
        for (size_t i = 0; i < A.size(); ++i)
            for (size_t j = 0; j < B.size(); ++j) {
                best = std::min(best, pt_seg(A[i], B[j], B[(j + 1) % B.size()]));
                best = std::min(best, pt_seg(B[j], A[i], A[(i + 1) % A.size()]));
            }
        return {false, best};
    }

} // namespace gjk_2d

int main() {
    using namespace gjk_2d;
    const double PI = std::acos(-1.0);
    std::cout << "============================================================\n";
    std::cout << "     GJK (Gilbert-Johnson-Keerthi) Convex Collision & Distance\n";
    std::cout << "============================================================\n\n";

    Polygon box1 = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    Polygon box2 = {{3, 1}, {7, 1}, {7, 5}, {3, 5}};
    Polygon box3 = {{6, 0}, {9, 0}, {9, 4}, {6, 4}};
    auto r12 = gjk_intersect_and_distance(box1, box2);
    auto r13 = gjk_intersect_and_distance(box1, box3);
    std::cout << "Box1 [0,4]x[0,4] vs Box2 [3,7]x[1,5]: intersect=" << (r12.intersecting ? "YES" : "NO")
              << ", dist=" << r12.distance << "\n";
    std::cout << "Box1 [0,4]x[0,4] vs Box3 [6,9]x[0,4]: intersect=" << (r13.intersecting ? "YES" : "NO")
              << ", dist=" << r13.distance << " (expected 2.0)\n";

    std::mt19937 rng(2026);
    auto random_convex_ngon = [&](int k, double cx, double cy, double rad) {
        std::vector<double> angs(k);
        for (double& a : angs) a = (rng() % 10000) / 10000.0 * 2.0 * PI;
        std::sort(angs.begin(), angs.end());
        Polygon poly;
        for (double a : angs) poly.push_back({cx + rad * std::cos(a), cy + rad * std::sin(a)});
        return poly;
    };

    bool ok = true;
    int hits = 0;
    for (int trial = 0; trial < 400 && ok; ++trial) {
        Polygon A = random_convex_ngon(5 + rng() % 6, (static_cast<int>(rng() % 100) - 50) * 0.2,
                                       (static_cast<int>(rng() % 100) - 50) * 0.2, 3.0 + rng() % 4);
        Polygon B = random_convex_ngon(5 + rng() % 6, (static_cast<int>(rng() % 100) - 50) * 0.2,
                                       (static_cast<int>(rng() % 100) - 50) * 0.2, 3.0 + rng() % 4);
        auto gjk = gjk_intersect_and_distance(A, B);
        auto sat = sat_reference(A, B);
        hits += gjk.intersecting;
        if (gjk.intersecting != sat.intersecting || std::fabs(gjk.distance - sat.distance) > 1e-5)
            ok = false;
    }

    std::cout << "\nRandom convex polygons (400 pairs, " << hits << " overlapping) vs SAT + segment distance: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
