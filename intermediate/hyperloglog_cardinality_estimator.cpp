/**
 * @file hyperloglog_cardinality_estimator.cpp
 * @brief HyperLogLog (HLL) probabilistic cardinality estimation algorithm in C++17.
 *
 * HyperLogLog (Flajolet et al., 2007) is an algorithm for the count-distinct problem,
 * estimating the number of unique elements in a data stream with exceptional space efficiency.
 * For example, estimating cardinalities up to billions with ~1.04 / sqrt(m) relative error
 * using only a few kilobytes of state.
 *
 * Mathematical Core:
 *   - Uses m = 2^b registers (typically b in [8, 14]).
 *   - Hashes items to uniform 64-bit integers.
 *   - The first b bits index the register j in [0, m - 1].
 *   - The remaining (64 - b) bits determine rho(w), the position of the first 1-bit (leading zeros + 1).
 *   - Update: M[j] = max(M[j], rho(w)).
 *   - Raw estimate:
 *       E = alpha_m * m^2 / sum_{j=0}^{m-1} (2^{-M[j]})
 *     where alpha_m is a bias-correction constant.
 *   - Small-range correction (Linear Counting):
 *       If E <= 2.5 * m and some registers are 0:
 *         E* = m * ln(m / V), where V is the count of zero registers.
 *
 * Complexity:
 *   - Insert: O(1) time.
 *   - Estimate: O(m) time.
 *   - Space: O(m) bytes (m registers of 8 bits each).
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <string>
#include <iomanip>
#include <unordered_set>

namespace sketches {

/**
 * @brief 64-bit FNV-1a Hash Function for strings.
 */
uint64_t fnv1a64(const std::string& str) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL;
    }
    return hash;
}

class HyperLogLog {
private:
    uint32_t b; // Number of register bits
    uint32_t m; // Number of registers = 2^b
    std::vector<uint8_t> registers;
    double alphaMM;

    static double computeAlpha(uint32_t m) {
        if (m == 16) return 0.673;
        if (m == 32) return 0.697;
        if (m == 64) return 0.709;
        return 0.7213 / (1.0 + 1.079 / m);
    }

    static uint8_t countLeadingZeros64(uint64_t x) {
        if (x == 0) return 64;
        return static_cast<uint8_t>(__builtin_clzll(x));
    }

public:
    explicit HyperLogLog(uint32_t precisionBits = 10)
        : b(precisionBits), m(1U << precisionBits), registers(m, 0) {
        double alpha = computeAlpha(m);
        alphaMM = alpha * m * m;
    }

    void add(const std::string& item) {
        uint64_t h = fnv1a64(item);
        uint32_t j = static_cast<uint32_t>(h >> (64 - b)); // First b bits as register index
        uint64_t w = h & ((1ULL << (64 - b)) - 1ULL);     // Remaining bits
        uint8_t leadingZeros = countLeadingZeros64(w);
        uint8_t rank = (w == 0) ? static_cast<uint8_t>(64 - b + 1) : static_cast<uint8_t>(leadingZeros - b + 1);

        if (rank > registers[j]) {
            registers[j] = rank;
        }
    }

    double estimate() const {
        double sum = 0.0;
        uint32_t zeroCount = 0;

        for (uint32_t j = 0; j < m; ++j) {
            sum += std::pow(2.0, -static_cast<double>(registers[j]));
            if (registers[j] == 0) {
                zeroCount++;
            }
        }

        double rawEstimate = alphaMM / sum;

        // Small range correction (Linear Counting)
        if (rawEstimate <= 2.5 * m && zeroCount > 0) {
            return static_cast<double>(m) * std::log(static_cast<double>(m) / zeroCount);
        }

        return rawEstimate;
    }

    uint32_t getNumRegisters() const { return m; }
    double getStandardError() const { return 1.04 / std::sqrt(m); }
};

} // namespace sketches

int main() {
    std::cout << "========================================\n";
    std::cout << "   HYPERLOGLOG CARDINALITY ESTIMATOR    \n";
    std::cout << "========================================\n\n";

    // Precision b = 10 -> m = 1024 registers (~1 KB memory)
    uint32_t precision = 10;
    sketches::HyperLogLog hll(precision);

    std::cout << "Configuration:\n";
    std::cout << "  - Precision bits b : " << precision << "\n";
    std::cout << "  - Number of bins m : " << hll.getNumRegisters() << "\n";
    std::cout << "  - Theoretical error: " << std::fixed << std::setprecision(2)
              << (hll.getStandardError() * 100.0) << "%\n\n";

    std::vector<int> streamCheckpoints = {100, 500, 2000, 10000, 50000};
    int currentId = 0;
    std::unordered_set<std::string> exactTracker;

    std::cout << std::left << std::setw(15) << "Checkpoint"
              << std::setw(15) << "True Unique"
              << std::setw(18) << "HLL Estimate"
              << "Relative Error\n";
    std::cout << std::string(62, '-') << "\n";

    for (int target : streamCheckpoints) {
        while (currentId < target) {
            std::string item = "user_session_token_" + std::to_string(currentId);
            hll.add(item);
            exactTracker.insert(item);
            currentId++;
        }

        double est = hll.estimate();
        double trueCount = static_cast<double>(exactTracker.size());
        double relErr = std::abs(est - trueCount) / trueCount * 100.0;

        std::cout << std::left << std::setw(15) << target
                  << std::setw(15) << exactTracker.size()
                  << std::setw(18) << std::fixed << std::setprecision(1) << est
                  << std::setprecision(2) << relErr << "%\n";
    }

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
