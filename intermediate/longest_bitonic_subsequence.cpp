/**
 * @file longest_bitonic_subsequence.cpp
 * @brief Computes the Longest Bitonic Subsequence (LBS) with path reconstruction.
 *
 * Definition:
 * A sequence is bitonic if it first strictly increases, and then strictly decreases.
 * More formally, a sequence x[0..k-1] is bitonic if there exists a peak index p (0 <= p < k)
 * such that:
 *   x[0] < x[1] < ... < x[p] > x[p+1] > ... > x[k-1]
 * A purely increasing or purely decreasing sequence is also valid (peak at boundary).
 *
 * Algorithm:
 * 1. Compute lis[i]: length of the longest strictly increasing subsequence ending at index i.
 * 2. Compute lds[i]: length of the longest strictly decreasing subsequence starting at index i.
 * 3. The maximum bitonic subsequence length is max(lis[i] + lds[i] - 1) over all 0 <= i < n.
 * 4. Reconstruct the full bitonic sequence around the optimal peak index.
 *
 * Time Complexity:
 * - O(N^2) dynamic programming with full predecessor tracking for reconstruction.
 * - O(N log N) using binary search / patience sorting.
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct BitonicResult {
    int length;
    int peakIndex;
    std::vector<int> sequence;
};

/**
 * @brief Finds the Longest Bitonic Subsequence and reconstructs the sequence.
 * @param arr Input array of integers.
 * @return BitonicResult containing max length, peak index, and the sequence.
 */
BitonicResult findLongestBitonicSubsequence(const std::vector<int>& arr) {
    int n = static_cast<int>(arr.size());
    if (n == 0) return {0, -1, {}};

    // lis[i] stores the length of LIS ending at arr[i]
    std::vector<int> lis(n, 1);
    std::vector<int> lisParent(n, -1);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (arr[j] < arr[i] && lis[j] + 1 > lis[i]) {
                lis[i] = lis[j] + 1;
                lisParent[i] = j;
            }
        }
    }

    // lds[i] stores the length of LDS starting at arr[i]
    std::vector<int> lds(n, 1);
    std::vector<int> ldsNext(n, -1);

    for (int i = n - 1; i >= 0; --i) {
        for (int j = n - 1; j > i; --j) {
            if (arr[j] < arr[i] && lds[j] + 1 > lds[i]) {
                lds[i] = lds[j] + 1;
                ldsNext[i] = j;
            }
        }
    }

    // Find the peak index maximizing lis[i] + lds[i] - 1
    int maxLen = 0;
    int bestPeak = 0;

    for (int i = 0; i < n; ++i) {
        int currentLen = lis[i] + lds[i] - 1;
        if (currentLen > maxLen) {
            maxLen = currentLen;
            bestPeak = i;
        }
    }

    // Reconstruct the increasing part up to peak
    std::vector<int> incPart;
    int curr = bestPeak;
    while (curr != -1) {
        incPart.push_back(arr[curr]);
        curr = lisParent[curr];
    }
    std::reverse(incPart.begin(), incPart.end());

    // Reconstruct the decreasing part from peak onwards
    std::vector<int> decPart;
    curr = ldsNext[bestPeak];
    while (curr != -1) {
        decPart.push_back(arr[curr]);
        curr = ldsNext[curr];
    }

    // Combine
    std::vector<int> fullSeq = incPart;
    fullSeq.insert(fullSeq.end(), decPart.begin(), decPart.end());

    return {maxLen, bestPeak, fullSeq};
}

void printVector(const std::string& label, const std::vector<int>& v) {
    std::cout << label << " [ ";
    for (int val : v) {
        std::cout << val << " ";
    }
    std::cout << "] (size " << v.size() << ")\n";
}

int main() {
    std::cout << "=== Longest Bitonic Subsequence (LBS) ===\n\n";

    // Test 1: Standard bitonic sequence
    std::vector<int> arr1 = {1, 11, 2, 10, 4, 5, 2, 1};
    printVector("Input Array 1:", arr1);
    auto res1 = findLongestBitonicSubsequence(arr1);
    std::cout << "  Max Bitonic Length: " << res1.length << '\n';
    std::cout << "  Peak Element:       " << arr1[res1.peakIndex] << " at index " << res1.peakIndex << '\n';
    printVector("  Optimal Sequence:  ", res1.sequence);
    std::cout << '\n';

    // Test 2: Strictly decreasing array
    std::vector<int> arr2 = {12, 11, 40, 5, 3, 1};
    printVector("Input Array 2:", arr2);
    auto res2 = findLongestBitonicSubsequence(arr2);
    std::cout << "  Max Bitonic Length: " << res2.length << '\n';
    std::cout << "  Peak Element:       " << arr2[res2.peakIndex] << " at index " << res2.peakIndex << '\n';
    printVector("  Optimal Sequence:  ", res2.sequence);
    std::cout << '\n';

    // Test 3: Mountain array
    std::vector<int> arr3 = {0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15};
    printVector("Input Array 3:", arr3);
    auto res3 = findLongestBitonicSubsequence(arr3);
    std::cout << "  Max Bitonic Length: " << res3.length << '\n';
    std::cout << "  Peak Element:       " << arr3[res3.peakIndex] << " at index " << res3.peakIndex << '\n';
    printVector("  Optimal Sequence:  ", res3.sequence);

    return 0;
}
