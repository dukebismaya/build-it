/**
 * @file ford_johnson_merge_insertion_sort.cpp
 * @brief Implements the Ford-Johnson (Merge-Insertion) sorting algorithm that minimizes comparisons.
 *
 * Background:
 * Published by Lester Ford and Selmer Johnson in 1959, the Merge-Insertion Sort was designed
 * to achieve the theoretical minimum number of comparisons needed to sort N elements,
 * approaching the information-theoretic lower bound ceil(log2(N!)).
 *
 * Algorithm Structure:
 * 1. Pairwise Comparisons:
 *    - Pair up adjacent elements (a_i, b_i) and sort each pair so a_i > b_i.
 *    - If N is odd, leave the last element as an unmatched straggler.
 * 2. Recursive Main Chain Sorting:
 *    - Recursively sort the larger elements {a_1, a_2, ..., a_k} to form the main chain.
 * 3. Pend Insertion via Jacobsthal Numbers:
 *    - Insert the partner of the smallest main-chain element (b_1) at the start for free.
 *    - Insert remaining smaller elements {b_i} into the main chain using binary search.
 *    - The insertion order follows blocks defined by Jacobsthal numbers (1, 3, 5, 11, 21, ...)
 *      processed backwards within each block to ensure optimal binary search range sizes (powers of 2 minus 1).
 *
 * Time Complexity: O(N log N)
 * Comparisons: Closest known to ceil(log2(N!))
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>

class FordJohnsonSort {
private:
    static long long comparisonCount;

    static bool compare(int a, int b) {
        comparisonCount++;
        return a < b;
    }

    // Binary search insertion
    static void binaryInsert(std::vector<int>& chain, int val, int bound) {
        int left = 0;
        int right = bound;

        while (left < right) {
            int mid = left + (right - left) / 2;
            if (compare(val, chain[mid])) {
                right = mid;
            } else {
                left = mid + 1;
            }
        }

        chain.insert(chain.begin() + left, val);
    }

    // Generates Jacobsthal sequence values up to limit: J(n) = J(n-1) + 2*J(n-2)
    static std::vector<int> generateJacobsthal(int limit) {
        std::vector<int> jSeq = {1, 3};
        while (jSeq.back() < limit) {
            int nextJ = jSeq.back() + 2 * jSeq[jSeq.size() - 2];
            jSeq.push_back(nextJ);
        }
        return jSeq;
    }

public:
    static std::vector<int> sort(std::vector<int> arr) {
        int n = static_cast<int>(arr.size());
        if (n <= 1) return arr;

        // Step 1: Pair up elements and compare
        struct Pair {
            int larger;
            int smaller;
        };

        std::vector<Pair> pairs;
        int straggler = -1;
        bool hasStraggler = (n % 2 != 0);

        for (int i = 0; i + 1 < n; i += 2) {
            if (compare(arr[i + 1], arr[i])) {
                pairs.push_back({arr[i], arr[i + 1]});
            } else {
                pairs.push_back({arr[i + 1], arr[i]});
            }
        }

        if (hasStraggler) {
            straggler = arr.back();
        }

        // Step 2: Recursively sort the larger elements
        std::vector<int> largerElements;
        for (const auto& p : pairs) {
            largerElements.push_back(p.larger);
        }

        std::vector<int> sortedLarger = sort(largerElements);

        // Reorder pairs to match sorted larger elements
        std::vector<int> mainChain;
        std::vector<int> pend;

        for (int val : sortedLarger) {
            mainChain.push_back(val);
            for (const auto& p : pairs) {
                if (p.larger == val) {
                    pend.push_back(p.smaller);
                    break;
                }
            }
        }

        // Insert first pend element into main chain (it is smaller than mainChain[0])
        mainChain.insert(mainChain.begin(), pend[0]);

        // Step 3: Insert remaining pend elements using Jacobsthal order
        auto jSeq = generateJacobsthal(static_cast<int>(pend.size()));
        int lastInsertedIdx = 1;

        for (int jVal : jSeq) {
            int currEnd = std::min(jVal, static_cast<int>(pend.size()));
            for (int k = currEnd - 1; k >= lastInsertedIdx; --k) {
                int valToInsert = pend[k];
                // Find upper bound in mainChain where partner resides
                int bound = static_cast<int>(mainChain.size());
                for (int b = 0; b < static_cast<int>(mainChain.size()); ++b) {
                    if (mainChain[b] == sortedLarger[k]) {
                        bound = b;
                        break;
                    }
                }
                binaryInsert(mainChain, valToInsert, bound);
            }
            lastInsertedIdx = currEnd;
            if (lastInsertedIdx >= static_cast<int>(pend.size())) break;
        }

        // Step 4: Insert straggler if present
        if (hasStraggler) {
            binaryInsert(mainChain, straggler, static_cast<int>(mainChain.size()));
        }

        return mainChain;
    }

    static long long getComparisonCount() {
        return comparisonCount;
    }

    static void resetComparisons() {
        comparisonCount = 0;
    }
};

long long FordJohnsonSort::comparisonCount = 0;

int main() {
    std::cout << "=== Ford-Johnson Merge-Insertion Sort (Algorithm 1959) ===\n\n";

    std::vector<int> data = {21, 5, 87, 34, 12, 9, 64, 45, 1, 73, 29};

    std::cout << "Original Array (Size " << data.size() << "): [ ";
    for (int v : data) std::cout << v << " ";
    std::cout << "]\n";

    FordJohnsonSort::resetComparisons();
    auto sorted = FordJohnsonSort::sort(data);
    long long actualComparisons = FordJohnsonSort::getComparisonCount();

    std::cout << "Sorted Array:               [ ";
    for (int v : sorted) std::cout << v << " ";
    std::cout << "]\n\n";

    // Theoretical bounds
    double nFactLog2 = 0;
    for (int i = 1; i <= static_cast<int>(data.size()); ++i) {
        nFactLog2 += std::log2(i);
    }
    long long infoTheoreticalMin = static_cast<long long>(std::ceil(nFactLog2));

    std::cout << "Comparison Efficiency Analysis:\n";
    std::cout << "  Actual Comparisons Used:       " << actualComparisons << '\n';
    std::cout << "  Information-Theoretic Min:     " << infoTheoreticalMin << " comparisons (ceil(log2(N!)))\n";
    std::cout << "  Standard N*log2(N):            " << std::fixed << std::setprecision(1) 
              << (data.size() * std::log2(data.size())) << '\n';

    return 0;
}
