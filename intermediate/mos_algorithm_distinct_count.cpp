/*
 * ============================================================================
 *  Mo's Algorithm for Range Queries (Offline)
 * ============================================================================
 *
 *  Mo's Algorithm answers multiple offline range queries on an array in
 *  O((N + Q) * sqrt(N)) time by reordering queries to minimize the number
 *  of element insertions and deletions to a running window.
 *
 *  Core Idea:
 *  - Sort queries by (L / block_size, R). Within the same block, alternate
 *    ascending/descending R order (Hilbert curve optimization variant).
 *  - Maintain a current window [curL, curR] and expand/shrink it one element
 *    at a time using add(index) and remove(index) operations.
 *  - Each element is added/removed O(sqrt(N)) times across all queries.
 *
 *  Problem Solved Here:
 *  "Count the number of distinct elements in each query range [L, R]."
 *
 *  Time Complexity:  O((N + Q) * sqrt(N))
 *  Space Complexity: O(N + Q)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iomanip>

namespace mos_algorithm {

struct Query {
    int left;
    int right;
    int index; /* Original index for result ordering */
};

class MoDistinctCount {
public:
    MoDistinctCount(const std::vector<int>& arr)
        : data(arr), n(static_cast<int>(arr.size())), distinctCount(0) {

        /* Coordinate compress values for frequency array */
        sorted = arr;
        std::sort(sorted.begin(), sorted.end());
        sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());

        compressed.resize(n);
        for (int i = 0; i < n; ++i) {
            compressed[i] = static_cast<int>(
                std::lower_bound(sorted.begin(), sorted.end(), arr[i]) - sorted.begin());
        }

        freq.assign(sorted.size(), 0);
    }

    std::vector<int> solve(std::vector<Query>& queries) {
        int q = static_cast<int>(queries.size());
        int blockSize = std::max(1, static_cast<int>(std::sqrt(n)));

        /* Sort queries using Mo's ordering */
        std::sort(queries.begin(), queries.end(),
            [blockSize](const Query& a, const Query& b) {
                int blockA = a.left / blockSize;
                int blockB = b.left / blockSize;
                if (blockA != blockB) return blockA < blockB;
                /* Alternate R direction for better cache performance */
                return (blockA & 1) ? (a.right > b.right) : (a.right < b.right);
            });

        std::vector<int> results(q);
        int curL = 0, curR = -1;

        for (auto& query : queries) {
            /* Expand/shrink window to match [query.left, query.right] */
            while (curR < query.right) {
                ++curR;
                add(curR);
            }
            while (curL > query.left) {
                --curL;
                add(curL);
            }
            while (curR > query.right) {
                remove(curR);
                --curR;
            }
            while (curL < query.left) {
                remove(curL);
                ++curL;
            }

            results[query.index] = distinctCount;
        }

        return results;
    }

private:
    std::vector<int> data;
    std::vector<int> compressed;
    std::vector<int> sorted;
    std::vector<int> freq;
    int n;
    int distinctCount;

    void add(int idx) {
        int c = compressed[idx];
        if (freq[c] == 0) ++distinctCount;
        ++freq[c];
    }

    void remove(int idx) {
        int c = compressed[idx];
        --freq[c];
        if (freq[c] == 0) --distinctCount;
    }
};

/* Brute force for verification */
int bruteDistinct(const std::vector<int>& arr, int l, int r) {
    std::vector<int> sub(arr.begin() + l, arr.begin() + r + 1);
    std::sort(sub.begin(), sub.end());
    sub.erase(std::unique(sub.begin(), sub.end()), sub.end());
    return static_cast<int>(sub.size());
}

} // namespace mos_algorithm

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Mo's Algorithm — Offline Distinct Count Range Queries\n";
    std::cout << "========================================================\n\n";

    std::vector<int> arr = {1, 3, 3, 4, 1, 2, 5, 3, 4, 1, 2, 2, 5, 1, 3};
    int n = static_cast<int>(arr.size());

    std::cout << "Array (" << n << " elements): [";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << (i + 1 < n ? ", " : "]\n\n");
    }

    std::vector<mos_algorithm::Query> queries = {
        {0, 4, 0}, {1, 6, 1}, {3, 9, 2}, {0, 14, 3},
        {5, 12, 4}, {2, 2, 5}, {7, 13, 6}, {0, 0, 7}
    };

    mos_algorithm::MoDistinctCount solver(arr);
    auto results = solver.solve(queries);

    /* Re-sort by original index for display */
    std::vector<mos_algorithm::Query> display = {
        {0, 4, 0}, {1, 6, 1}, {3, 9, 2}, {0, 14, 3},
        {5, 12, 4}, {2, 2, 5}, {7, 13, 6}, {0, 0, 7}
    };

    std::cout << "Query Results:\n";
    std::cout << std::setw(8) << "Query" << std::setw(12) << "[L, R]"
              << std::setw(12) << "Mo's" << std::setw(12) << "Brute"
              << std::setw(10) << "Match?" << "\n";
    std::cout << "------------------------------------------------------\n";

    bool allMatch = true;
    for (int i = 0; i < static_cast<int>(display.size()); ++i) {
        int brute = mos_algorithm::bruteDistinct(arr, display[i].left, display[i].right);
        bool match = (results[i] == brute);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(display[i].left) + ", " +
                            std::to_string(display[i].right) + "]";
        std::cout << std::setw(8) << (i + 1) << std::setw(12) << range
                  << std::setw(12) << results[i] << std::setw(12) << brute
                  << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "------------------------------------------------------\n";
    std::cout << "All queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    int blockSize = std::max(1, static_cast<int>(std::sqrt(n)));
    std::cout << "\nBlock size used: " << blockSize << " (sqrt(" << n << "))\n";
    std::cout << "Total queries: " << display.size() << "\n";
    std::cout << "Approximate operations: O((" << n << " + " << display.size()
              << ") * " << blockSize << ") = O("
              << (n + static_cast<int>(display.size())) * blockSize << ")\n";

    return 0;
}
