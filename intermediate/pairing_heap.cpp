/*
 * ============================================================
 *                     Pairing Heap
 * ============================================================
 *
 * Description:
 *   A pairing heap is a heap-ordered multi-way tree with very
 *   small constant factors. It is often faster in practice than
 *   binary or Fibonacci heaps for Dijkstra/Prim workloads.
 *
 * Representation (left-child / right-sibling):
 *   Each node has a pointer to its first child and next sibling.
 *
 * Operations:
 *   meld(a, b):  the larger root becomes the first child of the
 *                smaller root.                         O(1)
 *   push(x):     meld with a single node.              O(1)
 *   top():       root key.                             O(1)
 *   pop():       remove root, then combine its children with the
 *                TWO-PASS strategy:
 *                  pass 1: meld children in pairs left to right
 *                  pass 2: meld the results right to left
 *                                                      O(log N) amortized
 *
 * Complexity:
 *   push/meld/top O(1); pop O(log N) amortized. Decrease-key
 *   is o(log N) amortized (exact bound is an open problem).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <functional>
#include <climits>

namespace pairing {

    class PairingHeap {
        struct Node {
            int key;
            Node* child = nullptr;
            Node* sibling = nullptr;
            explicit Node(int k) : key(k) {}
        };
        Node* root = nullptr;
        int count = 0;

        static Node* meld(Node* a, Node* b) {
            if (!a) return b;
            if (!b) return a;
            if (b->key < a->key) std::swap(a, b);
            b->sibling = a->child;
            a->child = b;
            return a;
        }

        static Node* two_pass(Node* first) {
            std::vector<Node*> pairs;
            while (first) {
                Node* a = first;
                Node* b = a->sibling;
                first = b ? b->sibling : nullptr;
                a->sibling = nullptr;
                if (b) b->sibling = nullptr;
                pairs.push_back(meld(a, b));
            }
            Node* r = nullptr;
            for (auto it = pairs.rbegin(); it != pairs.rend(); ++it) r = meld(*it, r);
            return r;
        }

        static void destroy(Node* n) {
            // iterative to avoid deep recursion
            std::vector<Node*> st;
            if (n) st.push_back(n);
            while (!st.empty()) {
                Node* x = st.back(); st.pop_back();
                if (x->child) st.push_back(x->child);
                if (x->sibling) st.push_back(x->sibling);
                delete x;
            }
        }

    public:
        PairingHeap() = default;
        PairingHeap(const PairingHeap&) = delete;
        PairingHeap& operator=(const PairingHeap&) = delete;
        ~PairingHeap() { destroy(root); }

        bool empty() const { return !root; }
        int size() const { return count; }
        int top() const { return root->key; }
        void push(int k) { root = meld(root, new Node(k)); ++count; }
        void pop() {
            Node* old = root;
            root = two_pass(root->child);
            delete old;
            --count;
        }
        void meld(PairingHeap& other) {
            root = meld(root, other.root);
            count += other.count;
            other.root = nullptr;
            other.count = 0;
        }
    };

} // namespace pairing

int main() {
    using pairing::PairingHeap;
    std::cout << "============================================================\n";
    std::cout << "                       Pairing Heap\n";
    std::cout << "============================================================\n\n";

    PairingHeap h;
    for (int x : {42, 7, 19, 3, 88, 25, 1, 64}) h.push(x);
    std::cout << "Pop order: ";
    while (!h.empty()) { std::cout << h.top() << " "; h.pop(); }
    std::cout << "\n";

    // Randomized check vs std::priority_queue including melds
    std::mt19937 rng(11);
    const int K = 6;
    std::vector<PairingHeap> heaps(K);
    std::vector<std::priority_queue<int, std::vector<int>, std::greater<int>>> ref(K);
    bool ok = true;
    for (int op = 0; op < 300000 && ok; ++op) {
        int i = rng() % K, kind = rng() % 10;
        if (kind < 6) { int v = rng() % 1000000; heaps[i].push(v); ref[i].push(v); }
        else if (kind < 9) {
            if (heaps[i].empty() != ref[i].empty()) ok = false;
            else if (!ref[i].empty()) {
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
    std::cout << "\nRandom test (300000 ops, push/pop/meld): " << (ok ? "PASSED" : "FAILED") << "\n";

    // Heapsort benchmark
    const int N = 1000000;
    std::vector<int> data(N);
    for (auto& x : data) x = static_cast<int>(rng());
    auto t0 = std::chrono::steady_clock::now();
    PairingHeap ph;
    for (int x : data) ph.push(x);
    bool sorted = true; int prev = INT_MIN;
    while (!ph.empty()) { if (ph.top() < prev) sorted = false; prev = ph.top(); ph.pop(); }
    auto t1 = std::chrono::steady_clock::now();
    std::cout << "Heapsort of 1M ints: " << std::chrono::duration<double, std::milli>(t1 - t0).count()
              << " ms, sorted = " << (sorted ? "yes" : "no") << "\n";
    return 0;
}
