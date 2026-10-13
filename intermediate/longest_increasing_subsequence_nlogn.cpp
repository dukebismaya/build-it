/*
 * Problem Description:
 * Longest Increasing Subsequence (LIS) using Dynamic Programming with Binary Search.
 * Given an unsorted array of integers, find the length of the longest strictly increasing
 * subsequence (not necessarily contiguous).
 * This implementation uses the patience sorting approach:
 * - Maintain a "tails" array where tails[i] is the smallest tail element for an
 *   increasing subsequence of length i+1.
 * - For each element, binary search in tails to find its position.
 * - Also reconstructs the actual LIS, not just its length.
 * Time Complexity: O(N log N) where N is the number of elements.
 * Space Complexity: O(N).
 */

#include <iostream>
#include <vector>
#include <algorithm>

// Returns length and the actual LIS
std::pair<int, std::vector<int>> longestIncreasingSubsequence(const std::vector<int>& nums) {
    int n = nums.size();
    if (n == 0) return {0, {}};

    std::vector<int> tails;         // tails[i] = smallest tail for IS of length i+1
    std::vector<int> tailIndices;   // index in nums for each tail
    std::vector<int> predecessor(n, -1); // for reconstruction
    std::vector<int> positionInTails(n);

    for (int i = 0; i < n; ++i) {
        auto it = std::lower_bound(tails.begin(), tails.end(), nums[i]);
        int pos = it - tails.begin();

        if (it == tails.end()) {
            tails.push_back(nums[i]);
            tailIndices.push_back(i);
        } else {
            *it = nums[i];
            tailIndices[pos] = i;
        }

        positionInTails[i] = pos;
        if (pos > 0) {
            predecessor[i] = tailIndices[pos - 1];
        }
    }

    // Reconstruct LIS
    int lisLen = tails.size();
    std::vector<int> lis(lisLen);
    int k = tailIndices[lisLen - 1];
    for (int i = lisLen - 1; i >= 0; --i) {
        lis[i] = nums[k];
        k = predecessor[k];
    }

    return {lisLen, lis};
}

int main() {
    std::vector<int> arr = {10, 9, 2, 5, 3, 7, 101, 18};

    auto [length, lis] = longestIncreasingSubsequence(arr);

    std::cout << "Longest Increasing Subsequence (LIS) - O(N log N)\n\n";
    std::cout << "Array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n\nLIS Length: " << length << "\n";
    std::cout << "One possible LIS: ";
    for (int x : lis) std::cout << x << " ";
    std::cout << "\n";

    // Second test case
    std::vector<int> arr2 = {0, 1, 0, 3, 2, 3};
    auto [len2, lis2] = longestIncreasingSubsequence(arr2);
    std::cout << "\nArray: ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << "\nLIS Length: " << len2 << "\n";
    std::cout << "One possible LIS: ";
    for (int x : lis2) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
