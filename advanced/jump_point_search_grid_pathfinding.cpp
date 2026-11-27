/*
 * ============================================================
 *       Jump Point Search (JPS) Optimal Grid Pathfinding
 * ============================================================
 *
 * Description:
 *   On uniform-cost 8-connected 2D grids (with cardinal step
 *   cost 1 and diagonal step cost sqrt(2), no corner-cutting),
 *   standard A* wastes time expanding thousands of symmetric
 *   equal-length paths across open rooms.
 *
 *   Jump Point Search (Harabor & Grastien, AAAI 2011) eliminates
 *   path symmetry online with zero precomputation and zero extra
 *   memory:
 *     1. Neighbor Pruning: When expanding node x reached from
 *        parent(x) in direction (dx, dy), prune all neighbors
 *        that can be reached at least as cheaply from parent(x)
 *        without going through x, keeping only:
 *          - Natural neighbors (1 for cardinal, 3 for diagonal)
 *          - Forced neighbors caused by adjacent obstacles.
 *     2. Jumping: Instead of pushing each neighbor to the open
 *        heap, recursively slide ("jump") along direction (dx, dy)
 *        until hitting the goal, an obstacle, or a node with at
 *        least one forced neighbor (or, for diagonal jumps, a node
 *        whose horizontal or vertical jump hits a jump point).
 *
 *   Theorem (Harabor & Grastien): JPS always returns the exact
 *   optimal shortest path cost while pushing a fraction of nodes
 *   to the priority queue.
 *
 * Complexity:
 *   Same worst-case guarantee as A*, typically 5x-20x fewer heap
 *   operations on open grids.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>

namespace jps_pathfinding {

    struct PathResult {
        bool found;
        double cost;
        int heap_pushes;
    };

    class GridPathfinder {
        int H, W;
        std::vector<std::string> grid;

        bool passable(int r, int c) const {
            return r >= 0 && r < H && c >= 0 && c < W && grid[r][c] != '#';
        }

        // Strict no-corner-cutting move check from (r, c) by (dr, dc)
        bool can_step(int r, int c, int dr, int dc) const {
            if (!passable(r + dr, c + dc)) return false;
            if (dr != 0 && dc != 0)
                return passable(r + dr, c) && passable(r, c + dc);
            return true;
        }

        static double octile_heuristic(int r1, int c1, int r2, int c2) {
            int dr = std::abs(r1 - r2), dc = std::abs(c1 - c2);
            const double SQRT2 = std::sqrt(2.0);
            return (dr + dc) + (SQRT2 - 2.0) * std::min(dr, dc);
        }

        // Check if (r, c) stepped into along (dr, dc) has a forced neighbor
        bool has_forced(int r, int c, int dr, int dc) const {
            if (dr != 0 && dc != 0) {
                // With strict no-corner-cutting, diagonal moves have no forced neighbors;
                // turning around an obstacle corner happens at the cardinal sub-jumps.
                return false;
            } else if (dr != 0) {
                // Moving vertically: obstacle beside (r - dr, c +- 1) opens (r, c +- 1) / (r + dr, c +- 1)
                return (!passable(r - dr, c + 1) && passable(r, c + 1)) ||
                       (!passable(r - dr, c - 1) && passable(r, c - 1));
            } else {
                // Moving horizontally
                return (!passable(r + 1, c - dc) && passable(r + 1, c)) ||
                       (!passable(r - 1, c - dc) && passable(r - 1, c));
            }
        }

        // Jump from (r, c) in direction (dr, dc); returns packed r*W + c or -1
        int jump(int r, int c, int dr, int dc, int gr, int gc) const {
            int nr = r + dr, nc = c + dc;
            if (!can_step(r, c, dr, dc)) return -1;
            if (nr == gr && nc == gc) return nr * W + nc;
            if (has_forced(nr, nc, dr, dc)) return nr * W + nc;

            if (dr != 0 && dc != 0) {
                if (jump(nr, nc, dr, 0, gr, gc) != -1 ||
                    jump(nr, nc, 0, dc, gr, gc) != -1)
                    return nr * W + nc;
            }
            return jump(nr, nc, dr, dc, gr, gc);
        }

    public:
        explicit GridPathfinder(std::vector<std::string> g)
            : H(static_cast<int>(g.size())), W(static_cast<int>(g[0].size())), grid(std::move(g)) {}

        PathResult a_star(int sr, int sc, int gr, int gc) const {
            struct State {
                double f, g;
                int u;
                bool operator>(const State& o) const { return f > o.f; }
            };
            const double INF = 1e100, SQRT2 = std::sqrt(2.0);
            std::vector<double> dist(H * W, INF);
            std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

            int start = sr * W + sc, goal = gr * W + gc;
            dist[start] = 0.0;
            pq.push({octile_heuristic(sr, sc, gr, gc), 0.0, start});
            int pushes = 1;

            const int dirs[8][2] = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};
            while (!pq.empty()) {
                auto [f, g, u] = pq.top(); pq.pop();
                if (g > dist[u] + 1e-12) continue;
                if (u == goal) return {true, g, pushes};
                int r = u / W, c = u % W;
                for (const auto& [dr, dc] : dirs) {
                    if (!can_step(r, c, dr, dc)) continue;
                    double w = (dr != 0 && dc != 0) ? SQRT2 : 1.0;
                    int v = (r + dr) * W + (c + dc);
                    if (g + w < dist[v] - 1e-12) {
                        dist[v] = g + w;
                        pq.push({dist[v] + octile_heuristic(r + dr, c + dc, gr, gc), dist[v], v});
                        ++pushes;
                    }
                }
            }
            return {false, INF, pushes};
        }

        PathResult jump_point_search(int sr, int sc, int gr, int gc) const {
            struct State {
                double f, g;
                int u, pr, pc;
                bool operator>(const State& o) const { return f > o.f; }
            };
            const double INF = 1e100;
            std::vector<double> dist(H * W, INF);
            std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

            int start = sr * W + sc, goal = gr * W + gc;
            dist[start] = 0.0;
            pq.push({octile_heuristic(sr, sc, gr, gc), 0.0, start, sr, sc});
            int pushes = 1;

            while (!pq.empty()) {
                auto [f, g, u, pr, pc] = pq.top(); pq.pop();
                if (g > dist[u] + 1e-12) continue;
                if (u == goal) return {true, g, pushes};
                int r = u / W, c = u % W;

                std::vector<std::pair<int, int>> dirs;
                if (u == start) {
                    for (int dr = -1; dr <= 1; ++dr)
                        for (int dc = -1; dc <= 1; ++dc)
                            if (dr != 0 || dc != 0) dirs.push_back({dr, dc});
                } else {
                    int dr = (r > pr) - (r < pr);
                    int dc = (c > pc) - (c < pc);
                    if (dr != 0 && dc != 0) {
                        dirs.push_back({dr, dc});
                        dirs.push_back({dr, 0});
                        dirs.push_back({0, dc});
                    } else if (dr != 0) {
                        dirs.push_back({dr, 0});
                        if (!passable(r - dr, c + 1) && passable(r, c + 1)) {
                            dirs.push_back({0, 1});
                            dirs.push_back({dr, 1});
                        }
                        if (!passable(r - dr, c - 1) && passable(r, c - 1)) {
                            dirs.push_back({0, -1});
                            dirs.push_back({dr, -1});
                        }
                    } else {
                        dirs.push_back({0, dc});
                        if (!passable(r + 1, c - dc) && passable(r + 1, c)) {
                            dirs.push_back({1, 0});
                            dirs.push_back({1, dc});
                        }
                        if (!passable(r - 1, c - dc) && passable(r - 1, c)) {
                            dirs.push_back({-1, 0});
                            dirs.push_back({-1, dc});
                        }
                    }
                }

                for (const auto& [dr, dc] : dirs) {
                    int jp = jump(r, c, dr, dc, gr, gc);
                    if (jp == -1) continue;
                    int jr = jp / W, jc = jp % W;
                    double step_cost = octile_heuristic(r, c, jr, jc);
                    if (g + step_cost < dist[jp] - 1e-12) {
                        dist[jp] = g + step_cost;
                        pq.push({dist[jp] + octile_heuristic(jr, jc, gr, gc), dist[jp], jp, r, c});
                        ++pushes;
                    }
                }
            }
            return {false, INF, pushes};
        }
    };

} // namespace jps_pathfinding

int main() {
    using namespace jps_pathfinding;
    std::cout << "============================================================\n";
    std::cout << "       Jump Point Search (JPS) Optimal Grid Pathfinding\n";
    std::cout << "============================================================\n\n";

    std::mt19937 rng(2026);
    bool ok = true;
    long long total_astar_pushes = 0, total_jps_pushes = 0;

    for (int trial = 0; trial < 80 && ok; ++trial) {
        int H = 40, W = 40;
        std::vector<std::string> g(H, std::string(W, '.'));
        // Add random room walls and pillars
        for (int r = 0; r < H; ++r)
            for (int c = 0; c < W; ++c)
                if (rng() % 5 == 0) g[r][c] = '#';
        g[0][0] = '.';
        g[H - 1][W - 1] = '.';

        GridPathfinder pf(g);
        auto ra = pf.a_star(0, 0, H - 1, W - 1);
        auto rj = pf.jump_point_search(0, 0, H - 1, W - 1);
        total_astar_pushes += ra.heap_pushes;
        total_jps_pushes   += rj.heap_pushes;

        if (ra.found != rj.found) ok = false;
        if (ra.found && std::fabs(ra.cost - rj.cost) > 1e-8) ok = false;
    }

    std::cout << "80 random 40x40 obstacle grids:\n";
    std::cout << "  A*  total priority-queue pushes : " << total_astar_pushes << "\n";
    std::cout << "  JPS total priority-queue pushes : " << total_jps_pushes
              << " (" << std::fixed << std::setprecision(2)
              << (static_cast<double>(total_astar_pushes) / std::max(1LL, total_jps_pushes))
              << "x fewer heap operations)\n";
    std::cout << "Exact shortest-path optimality match (JPS == A*): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
