/*
 * ============================================================
 *       R-Tree Spatial Index for 2D Bounding-Box Queries
 * ============================================================
 *
 * Description:
 *   An R-Tree (Antonin Guttman, 1984) is a height-balanced
 *   B-tree-like spatial index over Axis-Aligned Bounding Boxes
 *   (AABBs), widely used in spatial databases (PostGIS, SQLite
 *   R*Tree), GIS map engines, and physics broadphase collision.
 *
 *   Operations:
 *     - Each internal node stores between m and M child pointers
 *       and their Minimum Bounding Rectangle (MBR).
 *     - ChooseLeaf descends by picking the child whose MBR requires
 *       the minimum area enlargement to include the new box (ties
 *       broken by smaller current area).
 *     - QuadraticSplit partitions M + 1 entries into two nodes by
 *       picking the pair with maximum dead space as seeds and
 *       greedily assigning remaining entries by largest preference
 *       difference.
 *     - Search prunes any subtree whose MBR does not intersect the
 *       query rectangle.
 *
 * Complexity:
 *   Insert: O(log_M N) average
 *   Range / Intersection query: O(log_M N + K) average for K hits
 *   Space: O(N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

namespace r_tree {

    struct Rect {
        double xmin, ymin, xmax, ymax;

        double area() const {
            return std::max(0.0, xmax - xmin) * std::max(0.0, ymax - ymin);
        }

        bool intersects(const Rect& o) const {
            return xmin <= o.xmax && xmax >= o.xmin && ymin <= o.ymax && ymax >= o.ymin;
        }

        static Rect combine(const Rect& a, const Rect& b) {
            return {std::min(a.xmin, b.xmin), std::min(a.ymin, b.ymin),
                    std::max(a.xmax, b.xmax), std::max(a.ymax, b.ymax)};
        }

        double enlargement(const Rect& o) const {
            return combine(*this, o).area() - area();
        }
    };

    class RTree {
        static constexpr int MAX_ENTRIES = 8;
        static constexpr int MIN_ENTRIES = 3;

        struct Entry {
            Rect mbr;
            int child = -1; // node index if internal, or payload ID if leaf
        };

        struct Node {
            bool is_leaf = true;
            std::vector<Entry> entries;
        };

        std::vector<Node> nodes_;
        int root_ = 0;

        static Rect compute_mbr(const Node& n) {
            Rect r = n.entries[0].mbr;
            for (size_t i = 1; i < n.entries.size(); ++i)
                r = Rect::combine(r, n.entries[i].mbr);
            return r;
        }

        // Guttman's Quadratic Split of an overflowing node (MAX_ENTRIES + 1 items)
        int split_node(int u) {
            std::vector<Entry> all = std::move(nodes_[u].entries);
            int total = static_cast<int>(all.size());
            int s1 = 0, s2 = 1;
            double worst_waste = -1.0;
            for (int i = 0; i < total; ++i)
                for (int j = i + 1; j < total; ++j) {
                    double waste = Rect::combine(all[i].mbr, all[j].mbr).area()
                                 - all[i].mbr.area() - all[j].mbr.area();
                    if (waste > worst_waste) { worst_waste = waste; s1 = i; s2 = j; }
                }

            int v = static_cast<int>(nodes_.size());
            nodes_.push_back(Node{nodes_[u].is_leaf, {}});
            nodes_[u].entries = {all[s1]};
            nodes_[v].entries = {all[s2]};
            Rect mbr_u = all[s1].mbr, mbr_v = all[s2].mbr;

            std::vector<bool> taken(total, false);
            taken[s1] = taken[s2] = true;
            int rem = total - 2;

            while (rem > 0) {
                if (static_cast<int>(nodes_[u].entries.size()) + rem == MIN_ENTRIES) {
                    for (int i = 0; i < total; ++i) if (!taken[i]) nodes_[u].entries.push_back(all[i]);
                    break;
                }
                if (static_cast<int>(nodes_[v].entries.size()) + rem == MIN_ENTRIES) {
                    for (int i = 0; i < total; ++i) if (!taken[i]) nodes_[v].entries.push_back(all[i]);
                    break;
                }
                int best_i = -1;
                double best_diff = -1.0;
                for (int i = 0; i < total; ++i) {
                    if (taken[i]) continue;
                    double d1 = mbr_u.enlargement(all[i].mbr);
                    double d2 = mbr_v.enlargement(all[i].mbr);
                    double diff = std::fabs(d1 - d2);
                    if (diff > best_diff) { best_diff = diff; best_i = i; }
                }
                taken[best_i] = true;
                --rem;
                double d1 = mbr_u.enlargement(all[best_i].mbr);
                double d2 = mbr_v.enlargement(all[best_i].mbr);
                if (d1 < d2 || (d1 == d2 && mbr_u.area() < mbr_v.area())) {
                    nodes_[u].entries.push_back(all[best_i]);
                    mbr_u = Rect::combine(mbr_u, all[best_i].mbr);
                } else {
                    nodes_[v].entries.push_back(all[best_i]);
                    mbr_v = Rect::combine(mbr_v, all[best_i].mbr);
                }
            }
            return v;
        }

        // Returns newly split sibling index, or -1 if no split
        int insert_rec(int u, const Entry& item) {
            if (nodes_[u].is_leaf) {
                nodes_[u].entries.push_back(item);
            } else {
                int best = 0;
                double best_enl = nodes_[u].entries[0].mbr.enlargement(item.mbr);
                for (int i = 1; i < static_cast<int>(nodes_[u].entries.size()); ++i) {
                    double enl = nodes_[u].entries[i].mbr.enlargement(item.mbr);
                    if (enl < best_enl || (enl == best_enl &&
                        nodes_[u].entries[i].mbr.area() < nodes_[u].entries[best].mbr.area())) {
                        best_enl = enl;
                        best = i;
                    }
                }
                int child = nodes_[u].entries[best].child;
                int split_sib = insert_rec(child, item);
                nodes_[u].entries[best].mbr = compute_mbr(nodes_[child]);
                if (split_sib != -1)
                    nodes_[u].entries.push_back({compute_mbr(nodes_[split_sib]), split_sib});
            }
            if (static_cast<int>(nodes_[u].entries.size()) > MAX_ENTRIES)
                return split_node(u);
            return -1;
        }

        void query_rec(int u, const Rect& q, std::vector<int>& out) const {
            const auto& n = nodes_[u];
            for (const auto& e : n.entries) {
                if (!e.mbr.intersects(q)) continue;
                if (n.is_leaf) out.push_back(e.child);
                else query_rec(e.child, q, out);
            }
        }

    public:
        RTree() { nodes_.push_back(Node{true, {}}); }

        void insert(const Rect& box, int id) {
            int split_sib = insert_rec(root_, {box, id});
            if (split_sib != -1) {
                int old_root = root_;
                root_ = static_cast<int>(nodes_.size());
                nodes_.push_back(Node{false, {
                    {compute_mbr(nodes_[old_root]), old_root},
                    {compute_mbr(nodes_[split_sib]), split_sib}
                }});
            }
        }

        std::vector<int> query_intersecting(const Rect& q) const {
            std::vector<int> out;
            query_rec(root_, q, out);
            std::sort(out.begin(), out.end());
            return out;
        }
    };

} // namespace r_tree

int main() {
    using namespace r_tree;
    std::cout << "============================================================\n";
    std::cout << "       R-Tree Spatial Index for 2D Bounding-Box Queries\n";
    std::cout << "============================================================\n\n";

    std::mt19937 rng(2026);
    const int N = 4000;
    RTree tree;
    std::vector<Rect> boxes(N);
    for (int i = 0; i < N; ++i) {
        double x = (rng() % 10000) * 0.1;
        double y = (rng() % 10000) * 0.1;
        double w = 1.0 + (rng() % 50) * 0.2;
        double h = 1.0 + (rng() % 50) * 0.2;
        boxes[i] = {x, y, x + w, y + h};
        tree.insert(boxes[i], i);
    }

    bool ok = true;
    size_t total_hits = 0;
    for (int q = 0; q < 500 && ok; ++q) {
        double x = (rng() % 10000) * 0.1;
        double y = (rng() % 10000) * 0.1;
        double w = 10.0 + (rng() % 200) * 0.5;
        double h = 10.0 + (rng() % 200) * 0.5;
        Rect qbox{x, y, x + w, y + h};

        auto got = tree.query_intersecting(qbox);
        total_hits += got.size();
        std::vector<int> brute;
        for (int i = 0; i < N; ++i)
            if (boxes[i].intersects(qbox)) brute.push_back(i);
        if (got != brute) ok = false;
    }

    std::cout << "Indexed " << N << " 2D bounding boxes; 500 window queries returned "
              << total_hits << " total intersections.\n";
    std::cout << "R-Tree spatial queries vs linear scan (500 queries): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
