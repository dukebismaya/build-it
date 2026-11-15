/*
 * ============================================================================
 *  Persistent Trie (Functional / Path-Copying Trie)
 * ============================================================================
 *
 *  A persistent trie maintains access to all historical versions of the trie
 *  after each insertion. Instead of mutating nodes in place, insertion creates
 *  new nodes along the insertion path (path copying), sharing unmodified
 *  subtrees with previous versions.
 *
 *  Applications:
 *  - Version control for string sets (rollback to any historical state).
 *  - "Maximum XOR in a range of insertions" — query the trie built from
 *    elements inserted in versions [l, r] to find the maximum XOR with a
 *    given value. Uses subtree counts to restrict to valid version ranges.
 *
 *  Problem Solved:
 *  Given a sequence of N integers inserted one by one (versions 1..N),
 *  answer queries: "What is the maximum XOR of query value X with any
 *  element inserted in versions [L, R]?"
 *
 *  Approach:
 *  - Build a persistent binary trie on the binary representations (30 bits).
 *  - Each node stores a count of elements passing through it.
 *  - To query range [L, R], we use version[R] - version[L-1] counts to
 *    determine which branches have valid elements.
 *
 *  Time Complexity:
 *  - Insert: O(B) per insertion, B = number of bits
 *  - Query:  O(B) per query
 *  Space Complexity: O(N * B) total nodes
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace persistent_trie {

constexpr int BITS = 30; /* Handle values up to 2^30 - 1 */

struct TrieNode {
    int children[2];
    int count; /* Number of elements passing through this node */

    TrieNode() : count(0) {
        children[0] = children[1] = 0;
    }
};

class PersistentTrie {
public:
    PersistentTrie() {
        /* Node 0 is the null/empty sentinel */
        nodes.emplace_back();
        /* Root of empty trie (version 0) */
        roots.push_back(0);
    }

    /* Insert value into the latest version, creating a new version */
    void insert(int value) {
        int prevRoot = roots.back();
        int newRoot = insertHelper(prevRoot, value, BITS - 1);
        roots.push_back(newRoot);
    }

    /*
     * Query: maximum XOR of queryVal with any element inserted in
     * versions [l, r] (1-indexed).
     * Uses version[r] and version[l-1] subtree counts.
     */
    int maxXorInRange(int l, int r, int queryVal) const {
        int nodeR = roots[r];
        int nodeL = roots[l - 1];
        return maxXorHelper(nodeR, nodeL, queryVal, BITS - 1);
    }

    int versionCount() const { return static_cast<int>(roots.size()) - 1; }

private:
    std::vector<TrieNode> nodes;
    std::vector<int> roots; /* roots[i] = root node index for version i */

    int newNode() {
        nodes.emplace_back();
        return static_cast<int>(nodes.size()) - 1;
    }

    int insertHelper(int prevNode, int value, int bit) {
        int cur = newNode();
        nodes[cur] = nodes[prevNode]; /* Copy from previous version */
        nodes[cur].count++;

        if (bit < 0) return cur;

        int b = (value >> bit) & 1;
        nodes[cur].children[b] = insertHelper(nodes[prevNode].children[b], value, bit - 1);

        return cur;
    }

    int maxXorHelper(int nodeR, int nodeL, int queryVal, int bit) const {
        if (bit < 0) return 0;

        int b = (queryVal >> bit) & 1;
        int desired = 1 - b; /* We want the opposite bit for maximum XOR */

        /* Check if the desired branch has elements in range [l, r] */
        int countDesired = nodes[nodes[nodeR].children[desired]].count
                         - nodes[nodes[nodeL].children[desired]].count;

        if (countDesired > 0) {
            return (1 << bit) | maxXorHelper(
                nodes[nodeR].children[desired],
                nodes[nodeL].children[desired],
                queryVal, bit - 1);
        } else {
            return maxXorHelper(
                nodes[nodeR].children[b],
                nodes[nodeL].children[b],
                queryVal, bit - 1);
        }
    }
};

/* Brute force for verification */
int bruteMaxXor(const std::vector<int>& elements, int l, int r, int queryVal) {
    int best = 0;
    for (int i = l - 1; i < r; ++i) {
        best = std::max(best, elements[i] ^ queryVal);
    }
    return best;
}

} // namespace persistent_trie

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Persistent Trie — Range Maximum XOR Queries\n";
    std::cout << "========================================================\n\n";

    std::vector<int> elements = {5, 12, 3, 8, 15, 7, 10, 1, 14, 6};
    int n = static_cast<int>(elements.size());

    persistent_trie::PersistentTrie trie;

    std::cout << "Inserting elements (building versions 1.." << n << "):\n  [";
    for (int i = 0; i < n; ++i) {
        std::cout << elements[i] << (i + 1 < n ? ", " : "]\n\n");
        trie.insert(elements[i]);
    }

    struct Query {
        int l, r, x;
    };

    std::vector<Query> queries = {
        {1, 5, 9},   /* Max XOR with 9 in elements[0..4] = {5,12,3,8,15} */
        {3, 8, 11},  /* Max XOR with 11 in elements[2..7] = {3,8,15,7,10,1} */
        {1, 10, 0},  /* Max XOR with 0 in all elements */
        {6, 10, 13}, /* Max XOR with 13 in elements[5..9] = {7,10,1,14,6} */
        {1, 1, 5},   /* Single element: 5 XOR 5 = 0 */
        {4, 7, 6},   /* elements[3..6] = {8,15,7,10} XOR 6 */
    };

    std::cout << "Range Maximum XOR Queries:\n";
    std::cout << std::setw(8) << "[L, R]" << std::setw(8) << "X"
              << std::setw(16) << "Persistent" << std::setw(12) << "Brute"
              << std::setw(10) << "Match?" << "\n";
    std::cout << "------------------------------------------------------\n";

    bool allMatch = true;
    for (auto& q : queries) {
        int ptResult = trie.maxXorInRange(q.l, q.r, q.x);
        int bfResult = persistent_trie::bruteMaxXor(elements, q.l, q.r, q.x);
        bool match = (ptResult == bfResult);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(q.l) + "," + std::to_string(q.r) + "]";
        std::cout << std::setw(8) << range << std::setw(8) << q.x
                  << std::setw(16) << ptResult << std::setw(12) << bfResult
                  << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "------------------------------------------------------\n";
    std::cout << "All queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    std::cout << "\nTotal trie versions: " << trie.versionCount() << "\n";

    return 0;
}
