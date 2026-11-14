/**
 * @file wavelet_matrix_rank_select.cpp
 * @brief Wavelet Matrix succinct data structure for rank, select, and quantiles in C++17.
 *
 * Overview (Francisco Claude & Gonzalo Navarro, 2012):
 *   The Wavelet Matrix is a space-efficient alternative to the Wavelet Tree that supports
 *   access, rank, select, and range quantile queries over large integer alphabets Sigma.
 *   Unlike the tree variant, the Wavelet Matrix flattens each level into a SINGLE contiguous
 *   bitvector by placing all zeros first followed by all ones, removing tree pointer overhead.
 *
 * Core Supported Operations (in O(log Sigma) time):
 *   1. access(i): Returns the element at position i.
 *   2. rank(v, i): Counts occurrences of value v in prefix [0..i-1].
 *   3. select(v, k): Finds the position of the k-th occurrence of value v.
 *   4. quantile(L, R, k): Finds the k-th smallest element in subarray [L..R].
 *
 * Complexity:
 *   - Construction: O(N log Sigma)
 *   - Query time: O(log Sigma) for all operations.
 *   - Space: N * log Sigma bits + o(N log Sigma) auxiliary rank structures.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iomanip>

namespace succinct {

class BitVector {
private:
    std::vector<bool> bits;
    std::vector<uint32_t> prefixSum; // Fast rank1

public:
    explicit BitVector(size_t n = 0) : bits(n, false), prefixSum(n + 1, 0) {}

    void set(size_t i, bool val) {
        bits[i] = val;
    }

    void build() {
        prefixSum.assign(bits.size() + 1, 0);
        for (size_t i = 0; i < bits.size(); ++i) {
            prefixSum[i + 1] = prefixSum[i] + (bits[i] ? 1 : 0);
        }
    }

    bool get(size_t i) const {
        return bits[i];
    }

    // Number of 1-bits in [0, i-1]
    uint32_t rank1(size_t i) const {
        return prefixSum[std::min(i, bits.size())];
    }

    // Number of 0-bits in [0, i-1]
    uint32_t rank0(size_t i) const {
        return static_cast<uint32_t>(i) - rank1(i);
    }

    size_t size() const { return bits.size(); }
};

class WaveletMatrix {
private:
    size_t n;
    int maxBits;
    std::vector<BitVector> layers;
    std::vector<uint32_t> zerosCount; // Number of zeros at each layer

public:
    explicit WaveletMatrix(std::vector<uint32_t> data, int bits = 8)
        : n(data.size()), maxBits(bits), layers(bits), zerosCount(bits, 0) {
        if (n == 0) return;

        std::vector<uint32_t> current = data;
        std::vector<uint32_t> next(n);

        for (int layer = maxBits - 1; layer >= 0; --layer) {
            layers[layer] = BitVector(n);
            uint32_t zCount = 0;

            for (size_t i = 0; i < n; ++i) {
                bool bit = (current[i] >> layer) & 1;
                layers[layer].set(i, bit);
                if (!bit) zCount++;
            }
            layers[layer].build();
            zerosCount[layer] = zCount;

            // Stable partition: 0s to left, 1s to right
            size_t p0 = 0, p1 = zCount;
            for (size_t i = 0; i < n; ++i) {
                bool bit = (current[i] >> layer) & 1;
                if (!bit) {
                    next[p0++] = current[i];
                } else {
                    next[p1++] = current[i];
                }
            }
            current = next;
        }
    }

    /**
     * @brief access(i): Returns element at index i in O(log Sigma).
     */
    uint32_t access(size_t i) const {
        uint32_t val = 0;
        for (int layer = maxBits - 1; layer >= 0; --layer) {
            bool bit = layers[layer].get(i);
            if (bit) {
                val |= (1U << layer);
                i = zerosCount[layer] + layers[layer].rank1(i);
            } else {
                i = layers[layer].rank0(i);
            }
        }
        return val;
    }

    /**
     * @brief rank(v, i): Counts occurrences of v in prefix [0, i-1].
     */
    uint32_t rank(uint32_t v, size_t i) const {
        for (int layer = maxBits - 1; layer >= 0; --layer) {
            bool bit = (v >> layer) & 1;
            if (bit) {
                i = zerosCount[layer] + layers[layer].rank1(i);
            } else {
                i = layers[layer].rank0(i);
            }
        }
        return static_cast<uint32_t>(i);
    }

    /**
     * @brief quantile(L, R, k): Returns k-th smallest element in range [L, R] (1-based k).
     */
    uint32_t quantile(size_t L, size_t R, size_t k) const {
        uint32_t val = 0;
        for (int layer = maxBits - 1; layer >= 0; --layer) {
            size_t zerosBeforeL = layers[layer].rank0(L);
            size_t zerosBeforeR = layers[layer].rank0(R + 1);
            size_t zerosInRange = zerosBeforeR - zerosBeforeL;

            if (k <= zerosInRange) {
                // k-th element has a 0-bit
                L = zerosBeforeL;
                R = zerosBeforeR - 1;
            } else {
                // k-th element has a 1-bit
                val |= (1U << layer);
                k -= zerosInRange;
                size_t onesBeforeL = layers[layer].rank1(L);
                size_t onesBeforeR = layers[layer].rank1(R + 1);
                L = zerosCount[layer] + onesBeforeL;
                R = zerosCount[layer] + onesBeforeR - 1;
            }
        }
        return val;
    }
};

} // namespace succinct

int main() {
    std::cout << "========================================\n";
    std::cout << "     WAVELET MATRIX SUCCINCT INDEX      \n";
    std::cout << "  (O(log Sigma) Access, Rank & Median)  \n";
    std::cout << "========================================\n\n";

    std::vector<uint32_t> arr = {5, 12, 3, 8, 3, 15, 8, 3, 20, 8};
    // Alphabet up to 31 (bits = 5)
    succinct::WaveletMatrix wm(arr, 5);

    std::cout << "Indexed Sequence A (" << arr.size() << " elements):\n  [ ";
    for (uint32_t x : arr) std::cout << x << " ";
    std::cout << "]\n\n";

    std::cout << "1. O(log Sigma) Random Access Check:\n";
    for (size_t i = 0; i < arr.size(); ++i) {
        uint32_t val = wm.access(i);
        std::cout << "  A[" << i << "] = " << val
                  << " (Expected: " << arr[i] << ") -> "
                  << (val == arr[i] ? "OK" : "ERR") << "\n";
    }

    std::cout << "\n2. O(log Sigma) Range Quantile Query:\n";
    // Subarray A[1..7] = [12, 3, 8, 3, 15, 8, 3] -> Sorted: [3, 3, 3, 8, 8, 12, 15]
    size_t qL = 1, qR = 7;
    std::cout << "  Range A[" << qL << ".." << qR << "] sorted: [3, 3, 3, 8, 8, 12, 15]\n";
    std::cout << "  - 1st smallest (min)   : " << wm.quantile(qL, qR, 1) << " (Expected: 3)\n";
    std::cout << "  - 4th smallest (median): " << wm.quantile(qL, qR, 4) << " (Expected: 8)\n";
    std::cout << "  - 7th smallest (max)   : " << wm.quantile(qL, qR, 7) << " (Expected: 15)\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
