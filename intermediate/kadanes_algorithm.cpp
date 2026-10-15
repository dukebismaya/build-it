/*
 * Program Description:
 * Kadane's Algorithm for Maximum Subarray Sum.
 * Computes the maximum sum of a contiguous subarray in O(N) time and O(1) extra space.
 *
 * Extended features implemented:
 * 1. Standard Kadane's with start and end indices reconstruction.
 * 2. Handles edge cases like all-negative arrays gracefully.
 * 3. Maximum Circular Subarray Sum using inverted Kadane's (Total - Min Subarray).
 *
 * Time Complexity: O(N)
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

struct SubarrayResult {
    long long maxSum;
    int startIndex;
    int endIndex;
};

// Standard Kadane's algorithm with range recovery
SubarrayResult kadaneWithIndices(const std::vector<int>& nums) {
    if (nums.empty()) return {0, -1, -1};

    long long currentMax = nums[0];
    long long globalMax = nums[0];
    int start = 0, end = 0, tempStart = 0;

    for (int i = 1; i < static_cast<int>(nums.size()); ++i) {
        if (nums[i] > currentMax + nums[i]) {
            currentMax = nums[i];
            tempStart = i;
        } else {
            currentMax += nums[i];
        }

        if (currentMax > globalMax) {
            globalMax = currentMax;
            start = tempStart;
            end = i;
        }
    }

    return {globalMax, start, end};
}

// Maximum circular subarray sum (wraps around array boundaries)
long long maxSubarraySumCircular(const std::vector<int>& nums) {
    if (nums.empty()) return 0;

    long long totalSum = 0;
    long long currMax = 0, maxKadane = LLONG_MIN;
    long long currMin = 0, minKadane = LLONG_MAX;

    for (int x : nums) {
        totalSum += x;

        // Kadane for max subarray
        currMax = std::max(static_cast<long long>(x), currMax + x);
        maxKadane = std::max(maxKadane, currMax);

        // Kadane for min subarray
        currMin = std::min(static_cast<long long>(x), currMin + x);
        minKadane = std::min(minKadane, currMin);
    }

    // If all numbers are negative, maxKadane is the maximum element (totalSum == minKadane)
    if (maxKadane < 0) {
        return maxKadane;
    }

    return std::max(maxKadane, totalSum - minKadane);
}

void printArray(const std::vector<int>& arr) {
    std::cout << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << (i + 1 < arr.size() ? ", " : "");
    }
    std::cout << "]";
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Kadane's Algorithm Suite          \n";
    std::cout << "==========================================\n\n";

    std::vector<std::vector<int>> testCases = {
        {-2, 1, -3, 4, -1, 2, 1, -5, 4},
        {5, 4, -1, 7, 8},
        {-5, -2, -8, -1, -4},
        {1, -2, 3, -2},
        {5, -3, 5}
    };

    for (size_t t = 0; t < testCases.size(); ++t) {
        const auto& arr = testCases[t];
        std::cout << "Test Case #" << (t + 1) << ": Array = ";
        printArray(arr);
        std::cout << "\n";

        SubarrayResult res = kadaneWithIndices(arr);
        std::cout << "  Max Linear Subarray Sum: " << res.maxSum << "\n";
        std::cout << "  Subarray Range: indices [" << res.startIndex << " .. " << res.endIndex << "]: ";
        std::cout << "[";
        for (int i = res.startIndex; i <= res.endIndex; ++i) {
            std::cout << arr[i] << (i < res.endIndex ? ", " : "");
        }
        std::cout << "]\n";

        long long circMax = maxSubarraySumCircular(arr);
        std::cout << "  Max Circular Subarray Sum: " << circMax << "\n\n";
    }

    return 0;
}
