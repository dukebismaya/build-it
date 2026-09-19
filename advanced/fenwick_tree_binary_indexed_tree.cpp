/*
 * Problem Description:
 * Implement a Fenwick Tree (Binary Indexed Tree) for range sum queries and point updates.
 * Provides O(log n) complexity for both update and prefix sum operations.
 */

#include <iostream>
#include <vector>

using namespace std;

class FenwickTree {
    vector<int> bit;
    int n;

public:
    FenwickTree(int n) {
        this->n = n;
        bit.assign(n + 1, 0);
    }

    FenwickTree(vector<int> const &a) : FenwickTree(a.size()) {
        for (size_t i = 0; i < a.size(); i++)
            add(i, a[i]);
    }

    int sum(int r) {
        int ret = 0;
        for (; r >= 0; r = (r & (r + 1)) - 1)
            ret += bit[r];
        return ret;
    }

    int sum(int l, int r) {
        return sum(r) - sum(l - 1);
    }

    void add(int idx, int delta) {
        for (; idx < n; idx = idx | (idx + 1))
            bit[idx] += delta;
    }
};

int main() {
    vector<int> freq = {2, 1, 1, 3, 2, 3, 4, 5, 6, 7, 8, 9};
    FenwickTree tree(freq);

    cout << "Sum of elements in arr[0..5] is " << tree.sum(0, 5) << "\n";

    tree.add(3, 6);
    cout << "Sum of elements in arr[0..5] after update is " << tree.sum(0, 5) << "\n";

    return 0;
}
