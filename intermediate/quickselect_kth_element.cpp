/*
 * Program Description:
 * Quickselect Algorithm (Hoare's Selection Algorithm) in C++.
 * Finds the K-th smallest (or K-th largest) element in an unsorted array
 * in average O(N) time and O(1) extra memory.
 *
 * Mechanics:
 * Like Quicksort, Quickselect chooses a pivot element and partitions the array:
 * - Elements <= pivot go to the left.
 * - Elements > pivot go to the right.
 * Unlike Quicksort, it only recurses into ONE partition (the side where index K lies).
 *
 * Time Complexity:
 * - Average: O(N)
 * - Worst Case: O(N^2) (mitigated via randomized pivot)
 * Space Complexity: O(1) in-place.
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

class QuickSelect {
    std::mt19937 rng;

    int partition(std::vector<int>& arr, int left, int right, int pivotIndex) {
        int pivotValue = arr[pivotIndex];
        std::swap(arr[pivotIndex], arr[right]);
        int storeIndex = left;

        for (int i = left; i < right; ++i) {
            if (arr[i] < pivotValue) {
                std::swap(arr[storeIndex], arr[i]);
                storeIndex++;
            }
        }

        std::swap(arr[right], arr[storeIndex]);
        return storeIndex;
    }

    int selectHelper(std::vector<int>& arr, int left, int right, int k) {
        if (left == right) return arr[left];

        std::uniform_int_distribution<int> dist(left, right);
        int pivotIndex = dist(rng);

        pivotIndex = partition(arr, left, right, pivotIndex);

        if (k == pivotIndex) {
            return arr[k];
        } else if (k < pivotIndex) {
            return selectHelper(arr, left, pivotIndex - 1, k);
        } else {
            return selectHelper(arr, pivotIndex + 1, right, k);
        }
    }

public:
    QuickSelect() : rng(42) {}

    // Finds k-th smallest element (1-indexed k)
    int kthSmallest(std::vector<int> arr, int k) {
        if (k < 1 || k > static_cast<int>(arr.size())) return -1;
        return selectHelper(arr, 0, arr.size() - 1, k - 1);
    }

    // Finds k-th largest element (1-indexed k)
    int kthLargest(std::vector<int> arr, int k) {
        int n = arr.size();
        if (k < 1 || k > n) return -1;
        // The k-th largest is the (n - k + 1)-th smallest
        return selectHelper(arr, 0, n - 1, n - k);
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Quickselect Algorithm (Average O(N)) \n";
    std::cout << "==========================================\n\n";

    std::vector<int> nums = {12, 3, 5, 7, 4, 19, 26, 1, 15};
    std::cout << "Original Array: ";
    for (int x : nums) std::cout << x << " ";
    std::cout << "\n\n";

    QuickSelect qs;

    // Test k-th smallest
    for (int k = 1; k <= 5; ++k) {
        int val = qs.kthSmallest(nums, k);
        std::cout << "  " << k << "-th smallest element: " << val << "\n";
    }

    std::cout << "\nTest k-th largest:\n";
    for (int k = 1; k <= 3; ++k) {
        int val = qs.kthLargest(nums, k);
        std::cout << "  " << k << "-th largest element : " << val << "\n";
    }

    // Verification by sorting
    std::vector<int> sorted = nums;
    std::sort(sorted.begin(), sorted.end());
    std::cout << "\nSorted reference array: ";
    for (int x : sorted) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
