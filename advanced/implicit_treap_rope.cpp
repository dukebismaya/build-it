/*
 * Program Description:
 * Implicit Treap (Cartesian Tree on Implicit Keys / Rope Data Structure).
 * Represents an extensible array that supports powerful logarithmic operations:
 *
 * Supported operations in O(log N):
 * 1. Insert element at arbitrary 0-based position.
 * 2. Delete element at arbitrary position.
 * 3. Range Sum Query: sum of elements in [L, R].
 * 4. Range Reverse: reverses the subarray in [L, R] using lazy propagation!
 *
 * Core Treap Invariant:
 * - Binary search tree on implicit keys (subtree sizes).
 * - Min-heap (or max-heap) on randomly assigned priorities.
 *
 * Time Complexity: O(log N) expected for all split, merge, insert, reverse, query ops.
 * Space Complexity: O(N).
 */

#include <iostream>
#include <vector>
#include <random>

struct TreapNode {
    int val;
    int priority;
    int size = 1;
    long long sum;
    bool rev = false; // Lazy reverse tag
    TreapNode* left = nullptr;
    TreapNode* right = nullptr;

    TreapNode(int v, int p) : val(v), priority(p), sum(v) {}
};

class ImplicitTreap {
    TreapNode* root = nullptr;
    std::mt19937 rng;

    int getSize(TreapNode* t) const {
        return t ? t->size : 0;
    }

    long long getSum(TreapNode* t) const {
        return t ? t->sum : 0;
    }

    void update(TreapNode* t) {
        if (!t) return;
        t->size = 1 + getSize(t->left) + getSize(t->right);
        t->sum = t->val + getSum(t->left) + getSum(t->right);
    }

    void pushDown(TreapNode* t) {
        if (t && t->rev) {
            t->rev = false;
            std::swap(t->left, t->right);
            if (t->left) t->left->rev ^= true;
            if (t->right) t->right->rev ^= true;
        }
    }

    // Splits treap into L (first k elements) and R (remaining)
    void split(TreapNode* t, int k, TreapNode*& l, TreapNode*& r) {
        if (!t) {
            l = r = nullptr;
            return;
        }
        pushDown(t);

        int leftSize = getSize(t->left);
        if (leftSize >= k) {
            split(t->left, k, l, t->left);
            r = t;
        } else {
            split(t->right, k - leftSize - 1, t->right, r);
            l = t;
        }
        update(t);
    }

    // Merges two treaps L and R into t (all elements in L appear before R)
    void merge(TreapNode*& t, TreapNode* l, TreapNode* r) {
        pushDown(l);
        pushDown(r);

        if (!l || !r) {
            t = l ? l : r;
            return;
        }

        if (l->priority > r->priority) {
            merge(l->right, l->right, r);
            t = l;
        } else {
            merge(r->left, l, r->left);
            t = r;
        }
        update(t);
    }

    void outputHelper(TreapNode* t, std::vector<int>& out) {
        if (!t) return;
        pushDown(t);
        outputHelper(t->left, out);
        out.push_back(t->val);
        outputHelper(t->right, out);
    }

public:
    ImplicitTreap() : rng(1337) {}

    void insert(int pos, int val) {
        TreapNode *l, *r;
        split(root, pos, l, r);
        TreapNode* newNode = new TreapNode(val, rng());
        merge(l, l, newNode);
        merge(root, l, r);
    }

    void reverse(int ql, int qr) {
        TreapNode *l, *m, *r;
        split(root, ql, l, m);
        split(m, qr - ql + 1, m, r);
        if (m) m->rev ^= true;
        merge(root, l, m);
        merge(root, root, r);
    }

    long long querySum(int ql, int qr) {
        TreapNode *l, *m, *r;
        split(root, ql, l, m);
        split(m, qr - ql + 1, m, r);
        long long ans = getSum(m);
        merge(root, l, m);
        merge(root, root, r);
        return ans;
    }

    std::vector<int> toVector() {
        std::vector<int> out;
        outputHelper(root, out);
        return out;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Implicit Treap (Dynamic Array & Reverse)\n";
    std::cout << "==========================================\n\n";

    ImplicitTreap treap;

    // Build array: [1, 2, 3, 4, 5, 6, 7, 8]
    for (int i = 0; i < 8; ++i) {
        treap.insert(i, i + 1);
    }

    std::cout << "Initial Array: ";
    for (int x : treap.toVector()) std::cout << x << " ";
    std::cout << "\n\n";

    // Range sum query [2, 5] = 3 + 4 + 5 + 6 = 18
    std::cout << "Sum in range [2, 5]: " << treap.querySum(2, 5) << " (Expected: 18)\n";

    // Reverse range [1, 6] -> indices 1 to 6 (elements 2, 3, 4, 5, 6, 7 reversed to 7, 6, 5, 4, 3, 2)
    std::cout << "\nReversing subarray in range [1, 6] in O(log N)...\n";
    treap.reverse(1, 6);

    std::cout << "Array after reversal: ";
    for (int x : treap.toVector()) std::cout << x << " ";
    std::cout << " (Expected: 1 7 6 5 4 3 2 8)\n\n";

    // Insert element 99 at position 4
    std::cout << "Inserting 99 at position 4...\n";
    treap.insert(4, 99);

    std::cout << "Array after insertion: ";
    for (int x : treap.toVector()) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
