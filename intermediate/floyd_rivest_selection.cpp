/**
 * @file floyd_rivest_selection.cpp
 * @brief Selects the k-th smallest element using the Floyd-Rivest algorithm (Algorithm 489).
 *
 * Background:
 * Published by Robert W. Floyd and Ronald L. Rivest in 1975, the Floyd-Rivest algorithm
 * is an optimization of Hoare's Quickselect. While Quickselect partitions using a single
 * pivot, Floyd-Rivest samples a small subset of size s, computes two bounds that bracket
 * the k-th element with high probability, and recursively restricts the partition range.
 *
 * Performance:
 * - Average comparisons: n + min(k, n-k) + o(n), which is strictly superior to Quickselect's 3.38n.
 * - Average Time Complexity: O(n) with very small constant factors.
 * - Space Complexity: O(1) auxiliary space (in-place rearrangement).
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <iomanip>

/**
 * @brief Selects the k-th smallest element (0-indexed) in array[left..right] in-place.
 * @param arr The vector of elements.
 * @param left Starting index.
 * @param right Ending index.
 * @param k The rank of the element to find (left <= k <= right).
 */
void floydRivestSelect(std::vector<int>& arr, int left, int right, int k) {
    while (right > left) {
        // If the subarray is large enough, use random sampling to bracket the k-th element
        if (right - left > 600) {
            double n = static_cast<double>(right - left + 1);
            double i = static_cast<double>(k - left + 1);
            double z = std::log(n);
            double s = 0.5 * std::exp(2.0 * z / 3.0);
            double sd = 0.5 * std::sqrt(z * s * (n - s) / n) * (i - n / 2.0 < 0 ? -1.0 : 1.0);

            int newLeft = std::max(left, static_cast<int>(k - i * s / n + sd));
            int newRight = std::min(right, static_cast<int>(k + (n - i) * s / n + sd));

            floydRivestSelect(arr, newLeft, newRight, k);
        }

        // Standard 2-way / 3-way partition using arr[k] as pivot
        int pivot = arr[k];
        int i = left;
        int j = right;
        std::swap(arr[left], arr[k]);

        if (arr[right] > pivot) {
            std::swap(arr[left], arr[right]);
        }

        while (i < j) {
            std::swap(arr[i], arr[j]);
            i++;
            j--;
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
        }

        if (arr[left] == pivot) {
            std::swap(arr[left], arr[j]);
        } else {
            j++;
            std::swap(arr[right], arr[j]);
        }

        // Adjust bounds
        if (j <= k) left = j + 1;
        if (k <= j) right = j - 1;
    }
}

/**
 * @brief Wrapper to find the k-th smallest element in the entire vector.
 * @param arr Copy of the input vector.
 * @param k 0-indexed rank.
 * @return The k-th smallest element.
 */
int findKthSmallest(std::vector<int> arr, int k) {
    if (k < 0 || k >= static_cast<int>(arr.size())) {
        throw std::out_of_range("k is out of range");
    }
    floydRivestSelect(arr, 0, static_cast<int>(arr.size()) - 1, k);
    return arr[k];
}

int main() {
    std::cout << "=== Floyd-Rivest Algorithm (Algorithm 489: SELECT) ===\n\n";

    // 1. Small demonstration
    std::vector<int> smallData = {12, 3, 5, 7, 4, 19, 26, 1, 9, 15, 2, 8};
    std::cout << "Original Array: ";
    for (int x : smallData) std::cout << x << " ";
    std::cout << '\n';

    std::vector<int> sortedData = smallData;
    std::sort(sortedData.begin(), sortedData.end());

    std::cout << "\nTesting all k ranks for small array:\n";
    std::cout << "  " << std::setw(6) << "Rank k"
              << std::setw(16) << "Floyd-Rivest"
              << std::setw(16) << "Sorted Expected"
              << std::setw(10) << "Match?" << '\n';
    std::cout << "  " << std::string(48, '-') << '\n';

    for (size_t k = 0; k < smallData.size(); ++k) {
        int val = findKthSmallest(smallData, static_cast<int>(k));
        int expected = sortedData[k];
        std::cout << "  " << std::setw(6) << k
                  << std::setw(16) << val
                  << std::setw(16) << expected
                  << std::setw(10) << (val == expected ? "PASS" : "FAIL") << '\n';
    }

    // 2. Large array test (triggering sampling branch where size > 600)
    const int N = 5000;
    std::vector<int> largeData(N);
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(-100000, 100000);
    for (int i = 0; i < N; ++i) {
        largeData[i] = dist(rng);
    }

    std::vector<int> largeSorted = largeData;
    std::sort(largeSorted.begin(), largeSorted.end());

    std::vector<int> testK = {0, 1, N / 4, N / 2, 3 * N / 4, N - 2, N - 1};
    std::cout << "\nTesting large array (N = " << N << ") with Floyd-Rivest sampling:\n";
    std::cout << "  " << std::setw(8) << "Rank k"
              << std::setw(16) << "Floyd-Rivest"
              << std::setw(16) << "std::sort"
              << std::setw(10) << "Match?" << '\n';
    std::cout << "  " << std::string(50, '-') << '\n';

    for (int k : testK) {
        int val = findKthSmallest(largeData, k);
        int expected = largeSorted[k];
        std::cout << "  " << std::setw(8) << k
                  << std::setw(16) << val
                  << std::setw(16) << expected
                  << std::setw(10) << (val == expected ? "PASS" : "FAIL") << '\n';
    }

    return 0;
}
