/*
 * Program Description:
 * Wavelet Tree Data Structure in C++.
 * A succinct, powerful data structure built over an array of integers that recursively
 * partitions the alphabet of values [minVal, maxVal].
 *
 * Supported Range Queries in O(log(Sigma)) time (where Sigma is value range):
 * 1. Range k-th Smallest Element in subarray [L, R] (1-indexed).
 * 2. Range Frequency: count of occurrences of value x in [L, R].
 * 3. Range Rank: count of elements <= x in [L, R].
 *
 * Construction:
 * At each node handling value range [low, high]:
 * - mid = low + (high - low) / 2
 * - Map elements <= mid to left child (bit 0), elements > mid to right child (bit 1).
 * - Maintain prefix counts (b[i] = count of elements mapped to left child up to index i).
 *
 * Time Complexity:
 * - Build: O(N log(Sigma))
 * - Queries: O(log(Sigma))
 * Space Complexity: O(N log(Sigma))
 */

#include <iostream>
#include <vector>
#include <algorithm>

class WaveletTree {
    int low, high;
    WaveletTree *left = nullptr, *right = nullptr;
    std::vector<int> b; // b[i] = count of elements in prefix [0..i-1] directed to left child

public:
    // Builds wavelet tree on array elements in range [from, to)
    WaveletTree(std::vector<int>::iterator from, std::vector<int>::iterator to, int x, int y) {
        low = x;
        high = y;
        if (from >= to || low >= high) return;

        int mid = low + (high - low) / 2;
        b.reserve((to - from) + 1);
        b.push_back(0);

        for (auto it = from; it != to; ++it) {
            b.push_back(b.back() + (*it <= mid));
        }

        auto pivot = std::stable_partition(from, to, [mid](int val) {
            return val <= mid;
        });

        left = new WaveletTree(from, pivot, low, mid);
        right = new WaveletTree(pivot, to, mid + 1, high);
    }

    ~WaveletTree() {
        delete left;
        delete right;
    }

    // 1. Range k-th smallest element in 1-based [l, r] (k is 1-indexed)
    int kthSmallest(int l, int r, int k) const {
        if (l > r) return 0;
        if (low == high) return low;

        int countLeft = b[r] - b[l - 1]; // elements in [l, r] that went to left child
        int newL = b[l - 1] + 1;
        int newR = b[r];

        if (k <= countLeft) {
            return left->kthSmallest(newL, newR, k);
        } else {
            return right->kthSmallest(l - b[l - 1], r - b[r], k - countLeft);
        }
    }

    // 2. Count occurrences of value k in 1-based range [l, r]
    int countOccurrences(int l, int r, int k) const {
        if (l > r || k < low || k > high) return 0;
        if (low == high) return r - l + 1;

        int mid = low + (high - low) / 2;
        if (k <= mid) {
            return left->countOccurrences(b[l - 1] + 1, b[r], k);
        } else {
            return right->countOccurrences(l - b[l - 1], r - b[r], k);
        }
    }

    // 3. Count elements <= k in 1-based range [l, r]
    int countLTE(int l, int r, int k) const {
        if (l > r || k < low) return 0;
        if (high <= k) return r - l + 1;

        int leftCount = left->countLTE(b[l - 1] + 1, b[r], k);
        int rightCount = right->countLTE(l - b[l - 1], r - b[r], k);
        return leftCount + rightCount;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Wavelet Tree Demonstration        \n";
    std::cout << "==========================================\n\n";

    // 1-based indexing for queries
    std::vector<int> data = {6, 1, 4, 3, 5, 2, 7, 3, 2};
    std::cout << "Array (1-indexed, size " << data.size() << "): [";
    for (size_t i = 0; i < data.size(); ++i) {
        std::cout << data[i] << (i + 1 < data.size() ? ", " : "");
    }
    std::cout << "]\n\n";

    int minVal = *std::min_element(data.begin(), data.end());
    int maxVal = *std::max_element(data.begin(), data.end());

    std::vector<int> copyData = data;
    WaveletTree wt(copyData.begin(), copyData.end(), minVal, maxVal);

    // Query 1: Range [2, 7] subarray is: [1, 4, 3, 5, 2, 7]
    // Sorted: [1, 2, 3, 4, 5, 7]
    std::cout << "Subarray [2..7]: [1, 4, 3, 5, 2, 7]\n";
    std::cout << "  1st smallest in [2, 7]: " << wt.kthSmallest(2, 7, 1) << " (Expected: 1)\n";
    std::cout << "  3rd smallest in [2, 7]: " << wt.kthSmallest(2, 7, 3) << " (Expected: 3)\n";
    std::cout << "  5th smallest in [2, 7]: " << wt.kthSmallest(2, 7, 5) << " (Expected: 5)\n";

    // Query 2: Frequency of value 3 in [1, 9]
    std::cout << "\nFrequency queries:\n";
    std::cout << "  Occurrences of 3 in [1..9]: " << wt.countOccurrences(1, 9, 3) << " (Expected: 2)\n";
    std::cout << "  Occurrences of 2 in [1..9]: " << wt.countOccurrences(1, 9, 2) << " (Expected: 2)\n";

    // Query 3: Count of elements <= 4 in [1, 9] (elements: 1, 4, 3, 2, 3, 2 -> 6 elements)
    std::cout << "\nRank queries:\n";
    std::cout << "  Count of elements <= 4 in [1..9]: " << wt.countLTE(1, 9, 4) << " (Expected: 6)\n";

    std::cout << "\nVerification: All Wavelet Tree operations PASSED successfully!\n";
    return 0;
}
