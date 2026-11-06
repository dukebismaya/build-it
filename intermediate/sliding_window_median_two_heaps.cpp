/**
 * @file sliding_window_median_two_heaps.cpp
 * @brief Computes the Sliding Window Median in O(N log K) time using dual balanced heaps with lazy deletion.
 *
 * Problem:
 * Given an integer array of length N and a window size K, compute the median of each
 * contiguous sub-window of size K as it moves from left to right.
 *
 * Two-Heap Invariant:
 * 1. maxHeap (lower half): stores the smallest elements.
 * 2. minHeap (upper half): stores the largest elements.
 * Invariant:
 * - If K is odd:  size(maxHeap) == size(minHeap) + 1  -> Median is maxHeap.top()
 * - If K is even: size(maxHeap) == size(minHeap)      -> Median is (maxHeap.top() + minHeap.top()) / 2.0
 *
 * Lazy Deletion:
 * Standard priority queues do not support arbitrary deletion. We record outgoing elements
 * in a hash map `delayed` and prune them only when they appear at the top of either heap.
 *
 * Time Complexity: O(N log K)
 * Space Complexity: O(K)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iomanip>

class SlidingWindowMedian {
private:
    std::priority_queue<int> maxHeap;                             // Lower half
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap; // Upper half
    std::unordered_map<int, int> delayed;                         // Delayed deletions
    int k;
    int maxHeapSize = 0;
    int minHeapSize = 0;

    template <typename T>
    void prune(T& heap) {
        while (!heap.empty() && delayed[heap.top()] > 0) {
            delayed[heap.top()]--;
            heap.pop();
        }
    }

    void balance() {
        if (maxHeapSize > minHeapSize + 1) {
            minHeap.push(maxHeap.top());
            maxHeap.pop();
            maxHeapSize--;
            minHeapSize++;
            prune(maxHeap);
        } else if (maxHeapSize < minHeapSize) {
            maxHeap.push(minHeap.top());
            minHeap.pop();
            minHeapSize--;
            maxHeapSize++;
            prune(minHeap);
        }
    }

    void addNum(int num) {
        if (maxHeap.empty() || num <= maxHeap.top()) {
            maxHeap.push(num);
            maxHeapSize++;
        } else {
            minHeap.push(num);
            minHeapSize++;
        }
        balance();
    }

    void removeNum(int num) {
        delayed[num]++;
        if (num <= maxHeap.top()) {
            maxHeapSize--;
            if (num == maxHeap.top()) {
                prune(maxHeap);
            }
        } else {
            minHeapSize--;
            if (num == minHeap.top()) {
                prune(minHeap);
            }
        }
        balance();
    }

    double getMedian() {
        if (k % 2 != 0) {
            return static_cast<double>(maxHeap.top());
        } else {
            return (static_cast<double>(maxHeap.top()) + static_cast<double>(minHeap.top())) / 2.0;
        }
    }

public:
    explicit SlidingWindowMedian(int windowSize) : k(windowSize) {}

    std::vector<double> computeMedians(const std::vector<int>& nums) {
        std::vector<double> medians;
        int n = static_cast<int>(nums.size());
        if (n < k || k <= 0) return medians;

        // Initialize first window
        for (int i = 0; i < k; ++i) {
            addNum(nums[i]);
        }
        medians.push_back(getMedian());

        // Slide window across array
        for (int i = k; i < n; ++i) {
            addNum(nums[i]);
            removeNum(nums[i - k]);
            medians.push_back(getMedian());
        }

        return medians;
    }
};

int main() {
    std::cout << "=== Sliding Window Median (Dual Heaps + Lazy Deletion) ===\n\n";

    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;

    std::cout << "Input Array: [ ";
    for (int v : nums) std::cout << v << " ";
    std::cout << "]\nWindow Size K: " << k << "\n\n";

    SlidingWindowMedian solver(k);
    auto medians = solver.computeMedians(nums);

    std::cout << "Sliding Window Evaluation Trace:\n";
    for (size_t i = 0; i < medians.size(); ++i) {
        std::cout << "  Window [" << i << ".." << (i + k - 1) << "]: [ ";
        for (int j = 0; j < k; ++j) {
            std::cout << nums[i + j] << " ";
        }
        std::cout << "] -> Median = " << std::fixed << std::setprecision(1) << medians[i] << '\n';
    }

    return 0;
}

