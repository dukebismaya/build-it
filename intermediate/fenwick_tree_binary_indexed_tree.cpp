/*
 * Problem Description:
 * Fenwick Tree (Binary Indexed Tree - BIT).
 * A data structure that can efficiently update elements and calculate prefix sums in an array of numbers.
 * Supports:
 * 1. Point update: Add a value delta to index i in O(log N) time.
 * 2. Prefix sum query: Calculate sum of elements in range [1, i] in O(log N) time.
 * 3. Range sum query: Calculate sum of elements in range [L, R] as query(R) - query(L - 1) in O(log N) time.
 * Space Complexity: O(N) auxiliary space.
 */

#include <iostream>
#include <vector>

class FenwickTree {
    int n;
    std::vector<long long> tree;

public:
    FenwickTree(int size) : n(size), tree(size + 1, 0) {}

    // Constructs BIT from an initial 1-indexed array in O(N) time
    FenwickTree(const std::vector<int>& arr) : n(arr.size()), tree(arr.size() + 1, 0) {
        for (int i = 1; i <= n; ++i) {
            tree[i] += arr[i - 1];
            int parent = i + (i & -i);
            if (parent <= n) {
                tree[parent] += tree[i];
            }
        }
    }

    // Adds delta to index i (1-based index)
    void add(int i, long long delta) {
        while (i <= n) {
            tree[i] += delta;
            i += (i & -i);
        }
    }

    // Returns prefix sum of elements from 1 to i
    long long query(int i) const {
        long long sum = 0;
        while (i > 0) {
            sum += tree[i];
            i -= (i & -i);
        }
        return sum;
    }

    // Returns sum in range [l, r] (1-based index)
    long long rangeQuery(int l, int r) const {
        if (l > r) return 0;
        return query(r) - query(l - 1);
    }
};

int main() {
    std::vector<int> numbers = {3, 2, -1, 6, 5, 4, -3, 3, 7, 2};
    FenwickTree bit(numbers);

    std::cout << "Fenwick Tree (Binary Indexed Tree) Demo:\n";
    std::cout << "Original array: ";
    for (int num : numbers) std::cout << num << " ";
    std::cout << "\n\n";

    std::cout << "Prefix sum up to index 5: " << bit.query(5) << " (expected 15)\n";
    std::cout << "Range sum [3, 7]: " << bit.rangeQuery(3, 7) << " (expected 11)\n";

    std::cout << "\nAdding 10 to index 4...\n";
    bit.add(4, 10);

    std::cout << "New range sum [3, 7]: " << bit.rangeQuery(3, 7) << " (expected 21)\n";
    std::cout << "New total sum [1, 10]: " << bit.rangeQuery(1, 10) << " (expected 38)\n";

    return 0;
}
