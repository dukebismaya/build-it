/*
 * ============================================================
 *           Min-Max Heap (Double-Ended Priority Queue)
 * ============================================================
 *
 * Description:
 *   A Min-Max Heap (Atkinson, Sack, Santoro, Strothotte, 1986)
 *   is an implicit complete binary tree stored in a single flat
 *   array that supports BOTH find-min and find-max in O(1) time,
 *   and insert, pop-min, and pop-max in O(log N) time.
 *
 *   Structure:
 *     - Nodes at EVEN depths (0, 2, 4, ...) are on MIN levels:
 *       every node is <= all nodes in its subtree.
 *     - Nodes at ODD depths (1, 3, 5, ...) are on MAX levels:
 *       every node is >= all nodes in its subtree.
 *     - The minimum is at the root (index 1).
 *     - The maximum is the larger of the root's two children
 *       (index 2 or 3).
 *
 *   Push-up and push-down compare a node against its parent
 *   (opposite level type) and then hop two levels at a time
 *   through grandparents or grandchildren (same level type).
 *
 * Complexity:
 *   min() / max():            O(1)
 *   push() / pop_min() / pop_max(): O(log N)
 *   Space:                    O(N) in a single contiguous vector
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <set>
#include <random>
#include <utility>
#include <algorithm>

namespace minmax_heap {

    class MinMaxHeap {
        std::vector<int> h{0};   // 1-indexed

        static bool is_min_level(int i) {
            int depth = 0;
            while (i > 1) { i >>= 1; ++depth; }
            return (depth & 1) == 0;
        }

        void push_up_same(int i, bool min_lvl) {
            while (i >= 4) {
                int gp = i >> 2;
                if ((min_lvl && h[i] < h[gp]) || (!min_lvl && h[i] > h[gp])) {
                    std::swap(h[i], h[gp]);
                    i = gp;
                } else break;
            }
        }

        void push_up(int i) {
            if (i <= 1) return;
            int p = i >> 1;
            if (is_min_level(i)) {
                if (h[i] > h[p]) { std::swap(h[i], h[p]); push_up_same(p, false); }
                else push_up_same(i, true);
            } else {
                if (h[i] < h[p]) { std::swap(h[i], h[p]); push_up_same(p, true); }
                else push_up_same(i, false);
            }
        }

        void push_down(int i) {
            int n = size();
            bool min_lvl = is_min_level(i);
            while (2 * i <= n) {
                // Collect children and grandchildren
                int best = -1;
                bool is_gc = false;
                auto better = [&](int a, int b) { return min_lvl ? (h[a] < h[b]) : (h[a] > h[b]); };

                for (int c : {2 * i, 2 * i + 1}) {
                    if (c <= n && (best == -1 || better(c, best))) { best = c; is_gc = false; }
                }
                for (int gc = 4 * i; gc <= 4 * i + 3 && gc <= n; ++gc) {
                    if (better(gc, best)) { best = gc; is_gc = true; }
                }

                if (is_gc) {
                    if (better(best, i)) {
                        std::swap(h[best], h[i]);
                        int p = best >> 1;
                        if ((min_lvl && h[best] > h[p]) || (!min_lvl && h[best] < h[p]))
                            std::swap(h[best], h[p]);
                        i = best;
                    } else break;
                } else {
                    if (better(best, i)) std::swap(h[best], h[i]);
                    break;
                }
            }
        }

        int max_index() const {
            int n = size();
            if (n == 1) return 1;
            if (n == 2) return 2;
            return (h[2] >= h[3]) ? 2 : 3;
        }

    public:
        int size() const { return static_cast<int>(h.size()) - 1; }
        bool empty() const { return size() == 0; }
        int min() const { return h[1]; }
        int max() const { return h[max_index()]; }

        void push(int x) {
            h.push_back(x);
            push_up(size());
        }

        void pop_min() {
            h[1] = h.back();
            h.pop_back();
            if (!empty()) push_down(1);
        }

        void pop_max() {
            int idx = max_index();
            h[idx] = h.back();
            h.pop_back();
            if (idx <= size()) push_down(idx);
        }
    };

} // namespace minmax_heap

int main() {
    using minmax_heap::MinMaxHeap;
    std::cout << "============================================================\n";
    std::cout << "           Min-Max Heap (Double-Ended Priority Queue)\n";
    std::cout << "============================================================\n\n";

    MinMaxHeap demo;
    for (int x : {40, 12, 77, 5, 93, 28, 61, 3, 84}) demo.push(x);
    std::cout << "Inserted {40, 12, 77, 5, 93, 28, 61, 3, 84}:\n";
    while (!demo.empty()) {
        std::cout << "  min = " << demo.min() << ", max = " << demo.max() << "  -> pop both\n";
        demo.pop_min();
        if (!demo.empty()) demo.pop_max();
    }

    std::mt19937 rng(2026);
    MinMaxHeap h;
    std::multiset<int> ref;
    bool ok = true;
    for (int op = 0; op < 200000 && ok; ++op) {
        int act = rng() % 10;
        if (act < 5 || ref.empty()) {
            int v = static_cast<int>(rng() % 100000);
            h.push(v);
            ref.insert(v);
        } else if (act < 7) {
            if (h.min() != *ref.begin()) ok = false;
            h.pop_min();
            ref.erase(ref.begin());
        } else {
            if (h.max() != *ref.rbegin()) ok = false;
            h.pop_max();
            ref.erase(std::prev(ref.end()));
        }
        if (!ref.empty()) {
            if (h.min() != *ref.begin() || h.max() != *ref.rbegin()) ok = false;
        }
    }
    std::cout << "\nRandom test vs std::multiset (200000 push/pop_min/pop_max ops): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
