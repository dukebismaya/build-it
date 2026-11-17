/**
 * ============================================================================
 * AHU Tree Isomorphism Algorithm (Aho-Hopcroft-Ullman Canonical Labeling)
 * ============================================================================
 * 
 * Description:
 * The AHU algorithm (devised by Alfred Aho, John Hopcroft, and Jeffrey Ullman
 * in 1974) determines whether two unrooted trees T_1 and T_2 of N vertices
 * are topologically isomorphic (identical graph structure up to vertex relabeling).
 * 
 * Algorithm Steps:
 * 1. Tree Centroid Identification:
 *    A tree has either 1 or 2 centroids (vertices whose removal divides the tree
 *    into connected components each having size at most N/2).
 *    Centroids are topological invariants: isomorphic trees must have the exact
 *    same number of centroids, and centroids must map to centroids.
 * 
 * 2. Rooting at Centroids:
 *    We root each tree at its centroid(s). If a tree has two centroids, we test
 *    both candidates.
 * 
 * 3. Canonical Subtree Serialization:
 *    For a rooted tree, we recursively construct a unique canonical string:
 *    - A leaf node returns "()".
 *    - An internal node collects the canonical representations of its children,
 *      sorts them lexicographically, and wraps their concatenation in parentheses:
 *        "(" + sorted_children_strings + ")"
 * 
 * 4. Isomorphism Check:
 *    T_1 and T_2 are isomorphic IF AND ONLY IF the set of canonical strings
 *    from T_1's centroids overlaps with the set of canonical strings from T_2's
 *    centroids!
 * 
 * Time Complexity:
 * - O(N log N) with string sorting / O(N) with integer radix remapping
 * - Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

namespace ahu {

class Tree {
public:
    int n;
    std::vector<std::vector<int>> adj;

    explicit Tree(int num_nodes) : n(num_nodes), adj(num_nodes) {}

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    /**
     * Finds the 1 or 2 centroids of the tree.
     */
    std::vector<int> findCentroids() const {
        std::vector<int> subtree_size(n, 0);
        std::vector<int> centroids;

        auto dfs_size = [&](auto& self, int u, int p) -> void {
            subtree_size[u] = 1;
            bool is_centroid = true;

            for (int v : adj[u]) {
                if (v != p) {
                    self(self, v, u);
                    subtree_size[u] += subtree_size[v];
                    if (subtree_size[v] > n / 2) {
                        is_centroid = false;
                    }
                }
            }

            if (n - subtree_size[u] > n / 2) {
                is_centroid = false;
            }

            if (is_centroid) {
                centroids.push_back(u);
            }
        };

        dfs_size(dfs_size, 0, -1);
        return centroids;
    }

    /**
     * Computes the canonical AHU string representation rooted at node root.
     */
    std::string getCanonicalString(int root) const {
        auto dfs_canonical = [&](auto& self, int u, int p) -> std::string {
            std::vector<std::string> child_strings;

            for (int v : adj[u]) {
                if (v != p) {
                    child_strings.push_back(self(self, v, u));
                }
            }

            // Sort children lexicographically for canonical invariant
            std::sort(child_strings.begin(), child_strings.end());

            std::string res = "(";
            for (const auto& s : child_strings) {
                res += s;
            }
            res += ")";
            return res;
        };

        return dfs_canonical(dfs_canonical, root, -1);
    }
};

/**
 * Tests whether two unrooted trees t1 and t2 are isomorphic.
 */
bool areIsomorphic(const Tree& t1, const Tree& t2) {
    if (t1.n != t2.n) return false;

    auto c1 = t1.findCentroids();
    auto c2 = t2.findCentroids();

    if (c1.size() != c2.size()) return false;

    // Check if any centroid root of t1 matches any centroid root of t2
    for (int r1 : c1) {
        std::string s1 = t1.getCanonicalString(r1);
        for (int r2 : c2) {
            std::string s2 = t2.getCanonicalString(r2);
            if (s1 == s2) {
                return true;
            }
        }
    }

    return false;
}

} // namespace ahu

int main() {
    std::cout << "========================================================\n";
    std::cout << " AHU Tree Isomorphism Algorithm (Aho-Hopcroft-Ullman)\n";
    std::cout << "========================================================\n\n";

    // Test 1: Two isomorphic trees with relabeled vertices
    // Tree 1: 0 - 1, 0 - 2, 1 - 3, 1 - 4
    // Tree 2: 4 - 2, 4 - 3, 2 - 0, 2 - 1
    int n1 = 5;
    ahu::Tree t1(n1);
    t1.addEdge(0, 1);
    t1.addEdge(0, 2);
    t1.addEdge(1, 3);
    t1.addEdge(1, 4);

    ahu::Tree t2(n1);
    t2.addEdge(4, 2);
    t2.addEdge(4, 3);
    t2.addEdge(2, 0);
    t2.addEdge(2, 1);

    std::cout << "Comparing Tree 1 and Tree 2 (both 5 vertices, relabeled):\n";
    auto c1 = t1.findCentroids();
    auto c2 = t2.findCentroids();

    std::cout << "  Tree 1 Centroids: ";
    for (int c : c1) std::cout << c << " ";
    std::cout << "| Canonical String: " << t1.getCanonicalString(c1[0]) << "\n";

    std::cout << "  Tree 2 Centroids: ";
    for (int c : c2) std::cout << c << " ";
    std::cout << "| Canonical String: " << t2.getCanonicalString(c2[0]) << "\n";

    bool iso12 = ahu::areIsomorphic(t1, t2);
    std::cout << "  Are T1 and T2 Isomorphic? "
              << (iso12 ? "YES (ISOMORPHIC)" : "NO") << "\n\n";

    // Test 2: Non-isomorphic tree with same vertex count (Path graph P_5)
    ahu::Tree t3(n1);
    t3.addEdge(0, 1);
    t3.addEdge(1, 2);
    t3.addEdge(2, 3);
    t3.addEdge(3, 4);

    std::cout << "Comparing Tree 1 and Tree 3 (Path graph P_5):\n";
    auto c3 = t3.findCentroids();
    std::cout << "  Tree 3 Centroids: ";
    for (int c : c3) std::cout << c << " ";
    std::cout << "| Canonical String: " << t3.getCanonicalString(c3[0]) << "\n";

    bool iso13 = ahu::areIsomorphic(t1, t3);
    std::cout << "  Are T1 and T3 Isomorphic? "
              << (iso13 ? "YES" : "NO (CORRECTLY DISTINGUISHED)") << "\n";

    return 0;
}
