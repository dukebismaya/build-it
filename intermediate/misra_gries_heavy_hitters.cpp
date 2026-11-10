/**
 * @file misra_gries_heavy_hitters.cpp
 * @brief Misra-Gries data stream frequent items (heavy hitters) algorithm in C++17.
 *
 * The Misra-Gries algorithm (1982) is a deterministic streaming algorithm that identifies
 * all elements occurring strictly more than m / k times in a stream of length m, using
 * only O(k) memory counters.
 *
 * Algorithm Details:
 *   - Maintain a dictionary D of at most (k - 1) key-counter pairs.
 *   - For each incoming element x in stream:
 *       1. If x in D: D[x]++
 *       2. Else if |D| < k - 1: D[x] = 1
 *       3. Else: Decrement all counters in D by 1. Remove all keys whose count drops to 0.
 *
 * Theoretical Guarantees:
 *   - Let f_x be the true frequency of item x in the stream of length m.
 *   - The estimated frequency satisfies: f_x - m/k <= estimate(x) <= f_x.
 *   - Any item with true frequency f_x > m/k is guaranteed to be present in D at the end.
 *   - An optional verification pass can determine exact counts of candidate items.
 *
 * Complexity:
 *   - Time: O(1) amortized per update (using hash map / decrement batching).
 *   - Space: O(k) counters.
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <cstdint>

namespace streaming {

template <typename T>
class MisraGries {
private:
    size_t k;
    size_t streamLength;
    std::unordered_map<T, int64_t> counters;

public:
    explicit MisraGries(size_t kParameter) : k(kParameter), streamLength(0) {
        if (k < 2) k = 2;
    }

    void process(const T& item) {
        streamLength++;

        // Case 1: Item already tracked
        auto it = counters.find(item);
        if (it != counters.end()) {
            it->second++;
            return;
        }

        // Case 2: Room in dictionary for a new counter
        if (counters.size() < k - 1) {
            counters[item] = 1;
            return;
        }

        // Case 3: Dictionary is full (has k-1 items) -> decrement all counters
        std::vector<T> toErase;
        for (auto& pair : counters) {
            pair.second--;
            if (pair.second == 0) {
                toErase.push_back(pair.first);
            }
        }
        for (const auto& key : toErase) {
            counters.erase(key);
        }
    }

    const std::unordered_map<T, int64_t>& getCandidates() const {
        return counters;
    }

    int64_t getEstimatedFrequency(const T& item) const {
        auto it = counters.find(item);
        return (it != counters.end()) ? it->second : 0;
    }

    size_t getStreamLength() const { return streamLength; }
    size_t getK() const { return k; }
    double getThreshold() const { return static_cast<double>(streamLength) / k; }
};

} // namespace streaming

int main() {
    std::cout << "========================================\n";
    std::cout << "  MISRA-GRIES STREAMING HEAVY HITTERS   \n";
    std::cout << "========================================\n\n";

    // Find items appearing more than 1/4 of total stream (k = 4 -> at most 3 counters)
    size_t k = 4;
    streaming::MisraGries<std::string> mg(k);

    // Stream elements: length 24
    // "alpha": 9 times (37.5%) -> Heavy hitter (> 24 / 4 = 6)
    // "beta":  7 times (29.2%) -> Heavy hitter (> 6)
    // "gamma": 4 times (16.7%) -> Borderline/Below threshold
    // "delta": 2 times
    // "epsilon": 2 times
    std::vector<std::string> stream = {
        "alpha", "beta", "alpha", "gamma", "alpha", "beta", "alpha", "alpha",
        "beta", "delta", "alpha", "beta", "gamma", "epsilon", "alpha", "beta",
        "alpha", "beta", "gamma", "alpha", "beta", "delta", "epsilon", "gamma"
    };

    std::unordered_map<std::string, int64_t> exactFrequencies;
    for (const auto& token : stream) {
        mg.process(token);
        exactFrequencies[token]++;
    }

    std::cout << "Stream processed: " << mg.getStreamLength() << " total tokens.\n";
    std::cout << "Parameter k = " << k << " (Threshold m/k = " << mg.getThreshold() << ")\n";
    std::cout << "Maximum counters maintained: " << (k - 1) << "\n\n";

    std::cout << std::left << std::setw(12) << "Item"
              << std::setw(14) << "True Count"
              << std::setw(16) << "MG Estimate"
              << std::setw(18) << "Heavy Hitter?"
              << "Guarantee Bound\n";
    std::cout << std::string(72, '-') << "\n";

    for (const auto& pair : exactFrequencies) {
        int64_t trueCount = pair.second;
        int64_t est = mg.getEstimatedFrequency(pair.first);
        bool isHeavy = (trueCount > static_cast<int64_t>(mg.getThreshold()));

        std::cout << std::left << std::setw(12) << pair.first
                  << std::setw(14) << trueCount
                  << std::setw(16) << est
                  << std::setw(18) << (isHeavy ? "YES (> m/k)" : "No")
                  << "[" << (trueCount - static_cast<int64_t>(mg.getThreshold()))
                  << " <= est <= " << trueCount << "]\n";
    }

    std::cout << "\nCandidates remaining in Misra-Gries summary:\n";
    for (const auto& pair : mg.getCandidates()) {
        std::cout << "  - '" << pair.first << "': counter = " << pair.second << "\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
