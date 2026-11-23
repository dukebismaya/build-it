/*
 * ============================================================
 *                 Leftist Heap (Mergeable Heap)
 * ============================================================
 *
 * Description:
 *   A leftist heap is a binary heap-ordered tree that supports
 *   MERGE in O(log N), which binary heaps cannot do efficiently.
 *
 *   Every node stores its rank (s-value) = length of the shortest
 *   path to a null child. The leftist property requires
 *     rank(left) >= rank(right)
 *   so the right spine has length O(log N).
 *
 * Operations:
 *   merge(a, b): keep the smaller root, merge its RIGHT subtree
 *                with the other heap, then swap children if the
 *                leftist property is violated.
 *   push(x)    = merge(heap, {x})
 *   pop()      = merge(left, right)
 *
 * Complexity:
 *   merge / push / pop: O(log N), top: O(1), space O(N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <utility>

namespace leftist {

    class LeftistHeap {
        struct Node {
            int key, rank = 1;
            Node *left = nullptr, *right = nullptr;
            explicit Node(int k) : key(k) {}
        };
        Node* root = nullptr;
        int count = 0;

        static int rank(Node* n) { return n ? n->rank : 0; }

        static Node* merge(Node* a, Node* b) {
            if (!a) return b;
            if (!b) return a;
            if (b->key < a->key) std::swap(a, b);
            a->right = merge(a->right, b);
            if (rank(a->left) < rank(a->right)) std::swap(a->left, a->right);
            a->rank = rank(a->right) + 1;
            return a;
        }

        static void destroy(Node* n) {
            if (!n) return;
            destroy(n->left);
            destroy(n->right);
            delete n;
        }

    public:
        LeftistHeap() = default;
        LeftistHeap(const LeftistHeap&) = delete;
        LeftistHeap& operator=(const LeftistHeap&) = delete;
        ~LeftistHeap() { destroy(root); }

        bool empty() const { return root == nullptr; }
        int size() const { return count; }
        int top() const { return root->key; }

        void push(int key) { root = merge(root, new Node(key)); ++count; }

        void pop() {
            Node* old = root;
            root = merge(root->left, root->right);
            delete old;
            --count;
        }

        // Absorb all elements of other (other becomes empty)
        void meld(LeftistHeap& other) {
            root = merge(root, other.root);
            count += other.count;
            other.root = nullptr;
            other.count = 0;
        }

        int right_spine_length() const {
            int len = 0;
            for (Node* n = root; n; n = n->right) ++len;
            return len;
        }
    };

} // namespace leftist

int main() {
    using leftist::LeftistHeap;
    std::cout << "============================================================\n";
    std::cout << "                Leftist Heap (Mergeable Heap)\n";
    std::cout << "============================================================\n\n";

    LeftistHeap a, b;
    for (int x : {15, 3, 22, 8, 1}) a.push(x);
    for (int x : {9, 4, 30, 2}) b.push(x);
    std::cout << "Heap A min = " << a.top() << ", Heap B min = " << b.top() << "\n";
    a.meld(b);
    std::cout << "After meld: size = " << a.size() << ", B empty = " << (b.empty() ? "yes" : "no") << "\n";
    std::cout << "Pop order: ";
    while (!a.empty()) { std::cout << a.top() << " "; a.pop(); }
    std::cout << "\n\n";

    // Randomized test vs std::priority_queue with interleaved melds
    std::mt19937 rng(77);
    const int K = 8;
    std::vector<LeftistHeap> heaps(K);
    std::vector<std::priority_queue<int, std::vector<int>, std::greater<int>>> ref(K);
    bool ok = true;
    for (int op = 0; op < 200000 && ok; ++op) {
        int i = rng() % K, kind = rng() % 10;
        if (kind < 6) {
            int v = rng() % 1000000;
            heaps[i].push(v); ref[i].push(v);
        } else if (kind < 9) {
            if (heaps[i].empty() != ref[i].empty()) ok = false;
            else if (!heaps[i].empty()) {
                if (heaps[i].top() != ref[i].top()) ok = false;
                heaps[i].pop(); ref[i].pop();
            }
        } else {
            int j = rng() % K;
            if (i == j) continue;
            heaps[i].meld(heaps[j]);
            while (!ref[j].empty()) { ref[i].push(ref[j].top()); ref[j].pop(); }
        }
    }
    std::cout << "Random test (200000 ops, 8 heaps, push/pop/meld): " << (ok ? "PASSED" : "FAILED") << "\n";

    LeftistHeap big;
    for (int i = 0; i < 1000000; ++i) big.push(static_cast<int>(rng() % 1000000000));
    std::cout << "1M elements: right spine length = " << big.right_spine_length()
              << " (bound: log2(N+1) ~ 20)\n";
    return 0;
}
