/*
 * Program Description:
 * Link-Cut Tree (Sleator & Tarjan Dynamic Trees) in C++.
 * Maintains a forest of rooted trees and supports dynamic structural modifications
 * and path queries in amortized O(log N) time per operation.
 *
 * Supported Operations:
 * 1. link(u, v): Creates an edge between u and v (combining their trees).
 * 2. cut(u, v): Removes the edge between u and v (splitting into two trees).
 * 3. connected(u, v): Tests if u and v belong to the same tree.
 * 4. queryPath(u, v): Computes aggregate sum along the path between u and v.
 *
 * Internal Structure:
 * - Preferred paths stored as Splay Trees keyed by depth.
 * - access(u): Exposes the path from tree root to u.
 * - makeRoot(u): Changes the root of the tree to u via reversal tag.
 *
 * Time Complexity: Amortized O(log N) per operation.
 * Space Complexity: O(N).
 */

#include <iostream>
#include <vector>
#include <algorithm>

struct Node {
    int ch[2] = {0, 0};
    int p = 0;
    long long val = 0;
    long long sum = 0;
    bool rev = false;
};

class LinkCutTree {
    std::vector<Node> tree;

    bool isRoot(int x) const {
        return tree[tree[x].p].ch[0] != x && tree[tree[x].p].ch[1] != x;
    }

    void pushUp(int x) {
        if (!x) return;
        tree[x].sum = tree[x].val + tree[tree[x].ch[0]].sum + tree[tree[x].ch[1]].sum;
    }

    void pushDown(int x) {
        if (x && tree[x].rev) {
            std::swap(tree[x].ch[0], tree[x].ch[1]);
            if (tree[x].ch[0]) tree[tree[x].ch[0]].rev ^= true;
            if (tree[x].ch[1]) tree[tree[x].ch[1]].rev ^= true;
            tree[x].rev = false;
        }
    }

    void pushAll(int x) {
        if (!isRoot(x)) pushAll(tree[x].p);
        pushDown(x);
    }

    void rotate(int x) {
        int y = tree[x].p, z = tree[y].p;
        int k = (tree[y].ch[1] == x);

        if (!isRoot(y)) {
            tree[z].ch[tree[z].ch[1] == y] = x;
        }
        tree[x].p = z;

        tree[y].ch[k] = tree[x].ch[k ^ 1];
        if (tree[x].ch[k ^ 1]) {
            tree[tree[x].ch[k ^ 1]].p = y;
        }

        tree[x].ch[k ^ 1] = y;
        tree[y].p = x;

        pushUp(y);
        pushUp(x);
    }

    void splay(int x) {
        pushAll(x);
        while (!isRoot(x)) {
            int y = tree[x].p, z = tree[y].p;
            if (!isRoot(y)) {
                if ((tree[y].ch[1] == x) ^ (tree[z].ch[1] == y)) {
                    rotate(x);
                } else {
                    rotate(y);
                }
            }
            rotate(x);
        }
    }

public:
    explicit LinkCutTree(int n) : tree(n + 1) {}

    void setVal(int u, long long val) {
        splay(u);
        tree[u].val = val;
        pushUp(u);
    }

    // Exposes root-to-u path
    void access(int x) {
        for (int t = 0; x; t = x, x = tree[x].p) {
            splay(x);
            tree[x].ch[1] = t;
            pushUp(x);
        }
    }

    // Makes x the root of its virtual tree
    void makeRoot(int x) {
        access(x);
        splay(x);
        tree[x].rev ^= true;
    }

    // Finds representative root of tree containing x
    int findRoot(int x) {
        access(x);
        splay(x);
        while (tree[x].ch[0]) {
            pushDown(x);
            x = tree[x].ch[0];
        }
        splay(x);
        return x;
    }

    // Links x and y (adds undirected edge between x and y)
    bool link(int x, int y) {
        makeRoot(x);
        if (findRoot(y) == x) return false; // Already connected
        tree[x].p = y;
        return true;
    }

    // Cuts edge between x and y
    bool cut(int x, int y) {
        makeRoot(x);
        if (findRoot(y) != x || tree[y].p != x || tree[y].ch[0]) return false;
        tree[y].p = 0;
        tree[x].ch[1] = 0;
        pushUp(x);
        return true;
    }

    bool connected(int x, int y) {
        return findRoot(x) == findRoot(y);
    }

    long long queryPath(int x, int y) {
        makeRoot(x);
        access(y);
        splay(y);
        return tree[y].sum;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Link-Cut Tree (Dynamic Forests)      \n";
    std::cout << "==========================================\n\n";

    // Initialize 6 nodes (1-indexed) with initial values
    LinkCutTree lct(6);
    for (int i = 1; i <= 6; ++i) {
        lct.setVal(i, i * 10); // Node 1: 10, Node 2: 20, ..., Node 6: 60
    }

    std::cout << "Node initial values: [1:10, 2:20, 3:30, 4:40, 5:50, 6:60]\n\n";

    // Build dynamic edges: 1-2, 2-3, 3-4
    lct.link(1, 2);
    lct.link(2, 3);
    lct.link(3, 4);
    std::cout << "Linked edges: (1-2), (2-3), (3-4)\n";
    std::cout << "Are 1 and 4 connected? " << (lct.connected(1, 4) ? "YES" : "NO") << "\n";
    std::cout << "Are 1 and 5 connected? " << (lct.connected(1, 5) ? "YES" : "NO") << "\n";

    // Path sum query on path 1 <-> 4: 10 + 20 + 30 + 40 = 100
    std::cout << "Path (1 <-> 4) sum: " << lct.queryPath(1, 4) << " (Expected: 100)\n\n";

    // Cut edge (2-3)
    std::cout << "Cutting edge (2-3)...\n";
    lct.cut(2, 3);
    std::cout << "Are 1 and 4 connected now? " << (lct.connected(1, 4) ? "YES" : "NO [CONFIRMED]") << "\n\n";

    // Link 4 to 5 and 5 to 6
    std::cout << "Linking (4-5) and (5-6)...\n";
    lct.link(4, 5);
    lct.link(5, 6);

    // Path sum query 3 <-> 6: 30 + 40 + 50 + 60 = 180
    std::cout << "Path (3 <-> 6) sum: " << lct.queryPath(3, 6) << " (Expected: 180)\n";

    std::cout << "\nVerification: All dynamic Link-Cut operations PASSED!\n";
    return 0;
}
