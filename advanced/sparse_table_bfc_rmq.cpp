/*
 * ============================================================================
 *  Sparse Table with Bender-Farach-Colton (BFC) for O(1) RMQ
 * ============================================================================
 *
 *  The standard sparse table answers idempotent range queries (min, max, gcd)
 *  in O(1) per query with O(N log N) preprocessing. This implementation
 *  demonstrates the full construction with floor-log precomputation and
 *  extends it with the Block Decomposition technique from Bender & Farach-
 *  Colton (2000) to achieve O(N) preprocessing for the RMQ problem on arrays
 *  derived from Euler tours (where consecutive elements differ by +/- 1).
 *
 *  Standard Sparse Table:
 *  - table[k][i] = min(arr[i..i+2^k-1])
 *  - query(l, r): let k = floor(log2(r - l + 1)), answer =
 *    min(table[k][l], table[k][r - 2^k + 1])
 *  - O(N log N) space and preprocessing, O(1) query.
 *
 *  BFC Enhancement (for +/- 1 RMQ):
 *  - Divide array into blocks of size b = floor(log2(N) / 2).
 *  - Each block's structure is characterized by its +/- 1 signature
 *    (at most 2^b distinct block types).
 *  - Precompute all possible answers for each block type.
 *  - Use a sparse table on block minimums for inter-block queries.
 *  - Total preprocessing: O(N), Query: O(1).
 *
 *  Time Complexity:
 *  - Standard: O(N log N) build, O(1) query
 *  - BFC:      O(N) build, O(1) query
 *  Space Complexity: O(N log N) for standard, O(N) for BFC
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <climits>

namespace rmq {

/* Floor log2 lookup table */
std::vector<int> buildLog2Table(int n) {
    std::vector<int> lg(n + 1, 0);
    for (int i = 2; i <= n; ++i) {
        lg[i] = lg[i / 2] + 1;
    }
    return lg;
}

class SparseTable {
public:
    SparseTable() : n(0) {}

    SparseTable(const std::vector<int>& arr) : n(static_cast<int>(arr.size())) {
        if (n == 0) return;
        lg = buildLog2Table(n);
        int maxLog = lg[n] + 1;

        table.resize(maxLog, std::vector<int>(n));
        idxTable.resize(maxLog, std::vector<int>(n));

        /* Level 0: each element alone */
        for (int i = 0; i < n; ++i) {
            table[0][i] = arr[i];
            idxTable[0][i] = i;
        }

        /* Build higher levels */
        for (int k = 1; k < maxLog; ++k) {
            for (int i = 0; i + (1 << k) - 1 < n; ++i) {
                if (table[k - 1][i] <= table[k - 1][i + (1 << (k - 1))]) {
                    table[k][i] = table[k - 1][i];
                    idxTable[k][i] = idxTable[k - 1][i];
                } else {
                    table[k][i] = table[k - 1][i + (1 << (k - 1))];
                    idxTable[k][i] = idxTable[k - 1][i + (1 << (k - 1))];
                }
            }
        }
    }

    int queryMin(int l, int r) const {
        int k = lg[r - l + 1];
        return std::min(table[k][l], table[k][r - (1 << k) + 1]);
    }

    int queryMinIdx(int l, int r) const {
        int k = lg[r - l + 1];
        if (table[k][l] <= table[k][r - (1 << k) + 1]) {
            return idxTable[k][l];
        }
        return idxTable[k][r - (1 << k) + 1];
    }

private:
    int n;
    std::vector<int> lg;
    std::vector<std::vector<int>> table, idxTable;
};

/*
 * BFC O(N) RMQ for +/- 1 arrays.
 * Assumes consecutive elements differ by exactly +1 or -1.
 */
class BFCRMQ {
public:
    BFCRMQ(const std::vector<int>& arr) : data(arr), n(static_cast<int>(arr.size())) {
        if (n == 0) return;

        blockSize = std::max(1, static_cast<int>(std::log2(n) / 2));
        numBlocks = (n + blockSize - 1) / blockSize;

        /* Compute block minimums and their positions */
        blockMin.resize(numBlocks);
        blockMinPos.resize(numBlocks);
        blockType.resize(numBlocks, 0);

        for (int b = 0; b < numBlocks; ++b) {
            int start = b * blockSize;
            int end = std::min(start + blockSize, n);
            blockMin[b] = data[start];
            blockMinPos[b] = start;

            /* Compute block type from +/- 1 signature */
            int sig = 0;
            for (int i = start + 1; i < end; ++i) {
                if (data[i] > data[i - 1]) {
                    sig |= (1 << (i - start - 1));
                }
                if (data[i] < blockMin[b]) {
                    blockMin[b] = data[i];
                    blockMinPos[b] = i;
                }
            }
            blockType[b] = sig;
        }

        /* Build sparse table on block minimums */
        interBlock = SparseTable(blockMin);

        /* Precompute intra-block lookup tables for each block type */
        int numTypes = 1 << (blockSize - 1);
        intraLookup.resize(numTypes);

        for (int b = 0; b < numBlocks; ++b) {
            int t = blockType[b];
            if (!intraLookup[t].empty()) continue;

            int len = std::min(blockSize, n - b * blockSize);
            /* Reconstruct a representative block of this type */
            std::vector<int> repr(len);
            repr[0] = 0;
            for (int i = 1; i < len; ++i) {
                repr[i] = repr[i - 1] + ((t & (1 << (i - 1))) ? 1 : -1);
            }

            /* Precompute min for all sub-ranges [i, j] */
            intraLookup[t].resize(len, std::vector<int>(len));
            for (int i = 0; i < len; ++i) {
                int minIdx = i;
                for (int j = i; j < len; ++j) {
                    if (repr[j] < repr[minIdx]) minIdx = j;
                    intraLookup[t][i][j] = minIdx;
                }
            }
        }
    }

    int queryMinIdx(int l, int r) const {
        int blockL = l / blockSize;
        int blockR = r / blockSize;

        if (blockL == blockR) {
            return intraQuery(blockL, l, r);
        }

        /* Left partial block */
        int bestIdx = intraQuery(blockL, l, (blockL + 1) * blockSize - 1);

        /* Full blocks in between */
        if (blockL + 1 <= blockR - 1) {
            int midBlockIdx = interBlock.queryMinIdx(blockL + 1, blockR - 1);
            if (data[blockMinPos[midBlockIdx]] < data[bestIdx]) {
                bestIdx = blockMinPos[midBlockIdx];
            }
        }

        /* Right partial block */
        int rightIdx = intraQuery(blockR, blockR * blockSize, r);
        if (data[rightIdx] < data[bestIdx]) {
            bestIdx = rightIdx;
        }

        return bestIdx;
    }

    int queryMin(int l, int r) const {
        return data[queryMinIdx(l, r)];
    }

private:
    std::vector<int> data;
    int n, blockSize, numBlocks;
    std::vector<int> blockMin, blockMinPos, blockType;
    SparseTable interBlock;
    std::vector<std::vector<std::vector<int>>> intraLookup;

    int intraQuery(int block, int l, int r) const {
        int start = block * blockSize;
        int localL = l - start;
        int localR = std::min(r - start, blockSize - 1);
        int localIdx = intraLookup[blockType[block]][localL][localR];
        return start + localIdx;
    }
};

/* Brute force */
int bruteMin(const std::vector<int>& arr, int l, int r) {
    int best = arr[l];
    for (int i = l + 1; i <= r; ++i) {
        best = std::min(best, arr[i]);
    }
    return best;
}

} // namespace rmq

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Sparse Table + BFC O(1) Range Minimum Queries\n";
    std::cout << "========================================================\n\n";

    /* Standard sparse table test */
    std::vector<int> arr = {7, 2, 3, 0, 5, 10, 3, 12, 18, 1, 4, 6};
    int n = static_cast<int>(arr.size());

    std::cout << "Array (" << n << "): [";
    for (int i = 0; i < n; ++i) {
        std::cout << arr[i] << (i + 1 < n ? ", " : "]\n\n");
    }

    rmq::SparseTable st(arr);

    struct Query { int l, r; };
    std::vector<Query> queries = {{0, 3}, {1, 5}, {4, 11}, {0, 11}, {9, 11}, {2, 2}, {3, 8}};

    std::cout << "Standard Sparse Table (O(1) RMQ):\n";
    std::cout << std::setw(10) << "[L, R]" << std::setw(8) << "ST"
              << std::setw(8) << "Brute" << std::setw(8) << "Match" << "\n";
    std::cout << "------------------------------------\n";

    bool allMatch = true;
    for (auto& q : queries) {
        int stResult = st.queryMin(q.l, q.r);
        int bfResult = rmq::bruteMin(arr, q.l, q.r);
        bool match = (stResult == bfResult);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(q.l) + "," + std::to_string(q.r) + "]";
        std::cout << std::setw(10) << range << std::setw(8) << stResult
                  << std::setw(8) << bfResult << std::setw(8) << (match ? "YES" : "NO") << "\n";
    }

    /* BFC RMQ on a +/-1 array (simulating Euler tour depths) */
    std::cout << "\nBFC O(N) RMQ on +/-1 Array:\n";
    std::vector<int> pm1 = {0, 1, 2, 1, 2, 3, 2, 1, 0, 1, 2, 1, 0, 1, 0};
    int m = static_cast<int>(pm1.size());

    std::cout << "Array (" << m << "): [";
    for (int i = 0; i < m; ++i) {
        std::cout << pm1[i] << (i + 1 < m ? ", " : "]\n");
    }

    rmq::BFCRMQ bfc(pm1);

    std::vector<Query> bfcQueries = {{0, 5}, {2, 8}, {0, 14}, {6, 12}, {3, 3}, {10, 14}};

    std::cout << std::setw(10) << "[L, R]" << std::setw(8) << "BFC"
              << std::setw(8) << "Brute" << std::setw(8) << "Match" << "\n";
    std::cout << "------------------------------------\n";

    for (auto& q : bfcQueries) {
        int bfcResult = bfc.queryMin(q.l, q.r);
        int bfResult = rmq::bruteMin(pm1, q.l, q.r);
        bool match = (bfcResult == bfResult);
        if (!match) allMatch = false;

        std::string range = "[" + std::to_string(q.l) + "," + std::to_string(q.r) + "]";
        std::cout << std::setw(10) << range << std::setw(8) << bfcResult
                  << std::setw(8) << bfResult << std::setw(8) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "\nAll queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    return 0;
}
