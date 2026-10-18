/*
 * Program Description:
 * Counting Inversions in an Array using Modified Merge Sort.
 * An inversion is a pair of indices (i, j) such that:
 * i < j and A[i] > A[j].
 *
 * It indicates how far (or close) an array is from being sorted:
 * - Already sorted array has 0 inversions.
 * - Completely reverse sorted array of size N has N * (N - 1) / 2 inversions.
 *
 * Modified Merge Sort Approach:
 * During the merge step of two sorted halves, if A[i] > A[j] (where i is in left half
 * and j is in right half), then ALL remaining elements in the left half from i to mid
 * are also greater than A[j].
 * Hence, inversions += (mid - i + 1).
 *
 * Time Complexity: O(N log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>

long long mergeAndCount(std::vector<int>& arr, std::vector<int>& temp, int left, int mid, int right) {
    int i = left;    // Pointer for left sub-array
    int j = mid + 1; // Pointer for right sub-array
    int k = left;    // Pointer for merged array
    long long invCount = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
            // There are (mid - i + 1) inversions because left subarray is sorted
            invCount += (mid - i + 1);
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = left; i <= right; ++i) {
        arr[i] = temp[i];
    }

    return invCount;
}

long long mergeSortAndCount(std::vector<int>& arr, std::vector<int>& temp, int left, int right) {
    long long invCount = 0;
    if (left < right) {
        int mid = left + (right - left) / 2;

        invCount += mergeSortAndCount(arr, temp, left, mid);
        invCount += mergeSortAndCount(arr, temp, mid + 1, right);
        invCount += mergeAndCount(arr, temp, left, mid, right);
    }
    return invCount;
}

long long countInversions(std::vector<int> arr) {
    std::vector<int> temp(arr.size());
    return mergeSortAndCount(arr, temp, 0, arr.size() - 1);
}

// O(N^2) brute force for verification
long long countInversionsBruteForce(const std::vector<int>& arr) {
    long long count = 0;
    int n = arr.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (arr[i] > arr[j]) count++;
        }
    }
    return count;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Inversion Counting via Merge Sort     \n";
    std::cout << "==========================================\n\n";

    std::vector<std::vector<int>> testCases = {
        {8, 4, 2, 1},
        {1, 20, 6, 4, 5},
        {1, 2, 3, 4, 5},
        {5, 4, 3, 2, 1},
        {2, 4, 1, 3, 5}
    };

    for (size_t t = 0; t < testCases.size(); ++t) {
        const auto& arr = testCases[t];
        std::cout << "Array #" << (t + 1) << ": [";
        for (size_t i = 0; i < arr.size(); ++i) {
            std::cout << arr[i] << (i + 1 < arr.size() ? ", " : "");
        }
        std::cout << "]\n";

        long long fastCount = countInversions(arr);
        long long bruteCount = countInversionsBruteForce(arr);

        std::cout << "  Merge Sort O(N log N) count : " << fastCount << "\n";
        std::cout << "  Brute Force O(N^2) count    : " << bruteCount << "\n";
        std::cout << "  Integrity Verification      : "
                  << (fastCount == bruteCount ? "MATCH [PASS]" : "MISMATCH") << "\n\n";
    }

    return 0;
}
