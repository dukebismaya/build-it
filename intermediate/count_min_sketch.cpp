/**
 * @file count_min_sketch.cpp
 * @brief Count-Min Sketch probabilistic frequency estimation in C++17.
 *
 * The Count-Min Sketch (Cormode & Muthukrishnan, 2005) is a sublinear space
 * probabilistic data structure for summarizing high-velocity data streams.
 *
 * Guarantees:
 *   Given error factor epsilon and failure probability delta:
 *     - Width w = ceil(e / epsilon)
 *     - Depth d = ceil(ln(1 / delta))
 *   Point Query:
 *     The estimated frequency a_hat(x) satisfies:
 *       a(x) <= a_hat(x) <= a(x) + epsilon * ||a||_1
 *     with probability at least (1 - delta).
 *
 * Complexity:
 *   - Update: O(d) hash computations and increments.
 *   - Query: O(d) hash lookups and minimum reductions.
 *   - Space: O(w * d) integer counters.
 */

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
#include <random>

namespace sketches {

class CountMinSketch {
private:
    uint32_t width;
    uint32_t depth;
    std::vector<std::vector<int64_t>> table;
    std::vector<uint64_t> hashA;
    std::vector<uint64_t> hashB;
    int64_t totalCount;
    static constexpr uint64_t PRIME = 2147483647ULL; // 2^31 - 1 Mersenne prime

    void initializeHashes(uint32_t seed) {
        std::mt19937_64 rng(seed);
        std::uniform_int_distribution<uint64_t> distA(1, PRIME - 1);
        std::uniform_int_distribution<uint64_t> distB(0, PRIME - 1);
        hashA.resize(depth);
        hashB.resize(depth);
        for (uint32_t i = 0; i < depth; ++i) {
            hashA[i] = distA(rng);
            hashB[i] = distB(rng);
        }
    }

    uint32_t hash(const std::string& str, uint32_t row) const {
        // FNV-1a basis
        uint64_t h = 14695981039346656037ULL;
        for (char c : str) {
            h ^= static_cast<uint64_t>(c);
            h *= 1099511628211ULL;
        }
        return static_cast<uint32_t>(((hashA[row] * h + hashB[row]) % PRIME) % width);
    }

public:
    CountMinSketch(double epsilon, double delta, uint32_t seed = 42)
        : totalCount(0) {
        width = static_cast<uint32_t>(std::ceil(std::exp(1.0) / epsilon));
        depth = static_cast<uint32_t>(std::ceil(std::log(1.0 / delta)));
        table.assign(depth, std::vector<int64_t>(width, 0));
        initializeHashes(seed);
    }

    void update(const std::string& item, int64_t count = 1) {
        totalCount += count;
        for (uint32_t i = 0; i < depth; ++i) {
            uint32_t col = hash(item, i);
            table[i][col] += count;
        }
    }

    int64_t estimate(const std::string& item) const {
        int64_t minCount = std::numeric_limits<int64_t>::max();
        for (uint32_t i = 0; i < depth; ++i) {
            uint32_t col = hash(item, i);
            minCount = std::min(minCount, table[i][col]);
        }
        return minCount;
    }

    uint32_t getWidth() const { return width; }
    uint32_t getDepth() const { return depth; }
    int64_t getTotalCount() const { return totalCount; }
};

} // namespace sketches

int main() {
    std::cout << "========================================\n";
    std::cout << "       COUNT-MIN SKETCH DEMO            \n";
    std::cout << "========================================\n\n";

    // epsilon = 0.01 (1% error), delta = 0.01 (99% confidence)
    sketches::CountMinSketch cms(0.01, 0.01);

    std::cout << "Count-Min Sketch Initialized:\n";
    std::cout << "  - Width (buckets per row) : " << cms.getWidth() << "\n";
    std::cout << "  - Depth (number of hashes): " << cms.getDepth() << "\n\n";

    std::unordered_map<std::string, int> trueCounts = {
        {"apple", 1500},
        {"banana", 3200},
        {"cherry", 450},
        {"date", 80},
        {"elderberry", 12},
        {"fig", 9500}
    };

    // Populate sketch with streaming items
    for (const auto& pair : trueCounts) {
        cms.update(pair.first, pair.second);
    }

    // Add noise items (stream of 10,000 background elements)
    for (int i = 0; i < 10000; ++i) {
        cms.update("noise_token_" + std::to_string(i % 100), 1);
    }

    std::cout << "Total items processed in stream: " << cms.getTotalCount() << "\n\n";
    std::cout << std::left << std::setw(15) << "Key"
              << std::setw(15) << "True Count"
              << std::setw(18) << "CMS Estimate"
              << "Overestimate Error\n";
    std::cout << std::string(62, '-') << "\n";

    for (const auto& pair : trueCounts) {
        int64_t est = cms.estimate(pair.first);
        int64_t diff = est - pair.second;

        std::cout << std::left << std::setw(15) << pair.first
                  << std::setw(15) << pair.second
                  << std::setw(18) << est
                  << "+" << diff << "\n";
    }

    // Unseen item query
    int64_t unseenEst = cms.estimate("grapefruit_unseen");
    std::cout << std::left << std::setw(15) << "grapefruit_unseen"
              << std::setw(15) << 0
              << std::setw(18) << unseenEst
              << "+" << unseenEst << "\n";

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
