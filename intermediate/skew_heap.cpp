/*
 * ============================================================
 *              Skew Heap (Self-Adjusting Heap)
 * ============================================================
 *
 * Description:
 *   A skew heap (Sleator & Tarjan, 1986) is a self-adjusting
 *   form of a leftist heap. Unlike a leftist heap, it stores NO
 *   rank / s-value in the nodes; instead, during every merge step
 *   along the right spine, it UNCONDITIONALLY swaps the left and
 *   right children of the merged root.
 *
 *   Merge(a, b):
 *     1. If a is null, return b; if b is null, return a.
 *     2. Ensure a->key <= b->key (swap a and b if needed).
 *     3. Recursively set a->right = Merge(a->right, b).
 *     4. Swap a->left and a->right.
 *     5. Return a.
 *
 * Complexity:
 *   Using a potential function Phi = number of heavy nodes on
 *   right paths:
 *     merge / push / pop: O(log N) amortized
 *     top:                O(1)
 *     space:              O(N) (no balance metadata per node)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <utility>
#include <functional>

namespace skew {

    class SkewHeap {
        struct Node {
            int key;
            Node *left = nullptr, *right = nullptr;
            explicit Node(int k) : key(k) {}
        };
        Node* root = nullptr;
        int count = 0;

        static Node* merge(Node* a, Node* b) {
            if (!a) return b;
            if (!b) return a;
            if (b->key < a->key) std::swap(a, b);
            a->right = merge(a->right, b);
            std::swap(a->left, a->right);   // unconditional skew swap
            return a;
        }

        static void destroy(Node* n) {
            std::vector<Node*> st;
            if (n) st.push_back(n);
            while (!st.empty()) {
                Node* cur = st.back(); st.pop_back();
                if (cur->left) st.push_back(cur->left);
                if (cur->right) st.push_back(cur->right);
                delete cur;
            }
        }

    public:
        SkewHeap() = default;
        SkewHeap(const SkewHeap&) = delete;
        SkewHeap& operator=(const SkewHeap&) = delete;
        ~SkewHeap() { destroy(root); }

        bool empty() const { return root == nullptr; }
        int size() const { return count; }
        int top() const { return root->key; }

        void push(int k) { root = merge(root, new Node(k)); ++count; }

        void pop() {
            Node* old = root;
            root = merge(root->left, root->right);
            delete old;
            --count;
        }

        void meld(SkewHeap& other) {
            root = merge(root, other.root);
            count += other.count;
            other.root = nullptr;
            other.count = 0;
        }
    };

} // namespace skew

int main() {
    using skew::SkewHeap;
    std::cout << "============================================================\n";
    std::cout << "               Skew Heap (Self-Adjusting Heap)\n";
    std::cout << "============================================================\n\n";

    SkewHeap h1, h2;
    for (int x : {19, 4, 27, 1, 13}) h1.push(x);
    for (int x : {8, 2, 33, 11}) h2.push(x);
    h1.meld(h2);
    std::cout << "Merged pop order: ";
    while (!h1.empty()) { std::cout << h1.top() << " "; h1.pop(); }
    std::cout << "\n";

    std::mt19937 rng(2025);
    const int K = 8;
    std::vector<SkewHeap> heaps(K);
    std::vector<std::priority_queue<int, std::vector<int>, std::greater<int>>> ref(K);
    bool ok = true;
    for (int op = 0; op < 250000 && ok; ++op) {
        int i = rng() % K, t = rng() % 10;
        if (t < 6) {
            int v = static_cast<int>(rng() % 1000000);
            heaps[i].push(v); ref[i].push(v);
        } else if (t < 9) {
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
    std::cout << "\nRandom test (250000 push/pop/meld ops across 8 heaps): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
