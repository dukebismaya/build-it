/*
 * Program Description:
 * Mo's Algorithm for Offline Range Queries in C++.
 * Solves range query problems where answers can be maintained under
 * add/remove operations in O(1) time.
 *
 * Algorithm Strategy:
 * 1. Divide array of size N into blocks of size B = sqrt(N).
 * 2. Group queries by their left endpoint block: (L / B).
 * 3. Sort queries:
 *    - Primary key: block index (L / B).
 *    - Secondary key: R coordinate (alternating asc/desc by block parity to optimize pointer travel).
 * 4. Move two pointers [curL, curR] to answer all Q queries offline.
 *
 * Example Problem:
 * Number of Distinct Elements in Subarray [L, R].
 *
 * Time Complexity: O((N + Q) * sqrt(N))
 * Space Complexity: O(N + Q)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

struct Query {
    int id;
    int l, r;
    int block;

    bool operator<(const Query& other) const {
        if (block != other.block) {
            return block < other.block;
        }
        // Parity optimization: sort odd blocks descending to reduce right-pointer rewind
        return (block & 1) ? (r < other.r) : (r > other.r);
    }
};

class MosAlgorithmDistinctElements {
    int n, q;
    int blockSize;
    std::vector<int> arr;
    std::vector<Query> queries;
    std::vector<int> freq;
    int distinctCount = 0;

    void add(int idx) {
        int val = arr[idx];
        if (freq[val] == 0) {
            distinctCount++;
        }
        freq[val]++;
    }

    void remove(int idx) {
        int val = arr[idx];
        freq[val]--;
        if (freq[val] == 0) {
            distinctCount--;
        }
    }

public:
    MosAlgorithmDistinctElements(const std::vector<int>& data, int maxVal)
        : n(data.size()), arr(data), freq(maxVal + 1, 0) {
        blockSize = std::max(1, static_cast<int>(std::sqrt(n)));
    }

    void addQuery(int id, int l, int r) {
        queries.push_back({id, l, r, l / blockSize});
    }

    std::vector<int> processQueries() {
        std::sort(queries.begin(), queries.end());
        std::vector<int> answers(queries.size());

        int curL = 0;
        int curR = -1;
        distinctCount = 0;

        for (const auto& qry : queries) {
            // Expand / shrink window to match [qry.l, qry.r]
            while (curR < qry.r) add(++curR);
            while (curL > qry.l) add(--curL);
            while (curR > qry.r) remove(curR--);
            while (curL < qry.l) remove(curL++);

            answers[qry.id] = distinctCount;
        }

        return answers;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Mo's Algorithm (Range Queries)      \n";
    std::cout << "==========================================\n\n";

    // Array with some duplicate elements
    std::vector<int> arr = {1, 1, 2, 1, 3, 4, 5, 2, 8};
    int maxVal = 10;

    std::cout << "Array (0-indexed): ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n\n";

    MosAlgorithmDistinctElements mo(arr, maxVal);

    // Queries: range [L, R]
    std::vector<std::pair<int, int>> queryRanges = {
        {0, 4}, // [1, 1, 2, 1, 3] -> 3 distinct (1, 2, 3)
        {1, 3}, // [1, 2, 1]       -> 2 distinct (1, 2)
        {2, 6}, // [2, 1, 3, 4, 5] -> 5 distinct (2, 1, 3, 4, 5)
        {0, 8}  // Entire array    -> 6 distinct (1, 2, 3, 4, 5, 8)
    };

    for (size_t i = 0; i < queryRanges.size(); ++i) {
        mo.addQuery(i, queryRanges[i].first, queryRanges[i].second);
    }

    auto answers = mo.processQueries();

    std::cout << "--- Offline Query Results (Distinct Elements in [L, R]) ---\n";
    for (size_t i = 0; i < queryRanges.size(); ++i) {
        std::cout << "Query #" << i << ": Range ["
                  << queryRanges[i].first << " .. " << queryRanges[i].second
                  << "] -> Distinct Count = " << answers[i] << "\n";
    }

    return 0;
}
