/**
 * @file aliquot_sequence_analyzer.cpp
 * @brief Aliquot sequence generator and cycle classification in C++17.
 *
 * Definition:
 *   For a positive integer n, let s(n) = sigma(n) - n denote the sum of its proper divisors.
 *   An aliquot sequence starting at n_0 is defined by the recurrence:
 *     n_{k+1} = s(n_k).
 *
 * Behaviors & Classifications:
 *   1. Terminating: The sequence hits a prime number p, then s(p) = 1, then s(1) = 0.
 *      Example: 12 -> 16 -> 15 -> 9 -> 4 -> 3 -> 1 -> 0.
 *   2. Perfect Number (Period 1): s(n) = n.
 *      Example: 6 -> 6 -> 6 ... or 28 -> 28 ...
 *   3. Amicable Pair (Period 2): s(a) = b and s(b) = a with a != b.
 *      Example: 220 <-> 284.
 *   4. Sociable Cycle (Period k >= 3): The sequence returns to the starting value after k steps.
 *
 * Complexity:
 *   - Proper divisor sum: O(sqrt(n)) time.
 *   - Sequence tracking: O(K * sqrt(M)) where K is sequence length and M is max term.
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>
#include <cstdint>
#include <string>

namespace number_theory {

/**
 * @brief Computes the sum of proper divisors of n: s(n) = sigma(n) - n.
 */
uint64_t sumProperDivisors(uint64_t n) {
    if (n <= 1) return 0;
    uint64_t sum = 1; // 1 is always a proper divisor

    for (uint64_t d = 2; d * d <= n; ++d) {
        if (n % d == 0) {
            sum += d;
            uint64_t other = n / d;
            if (other != d) {
                sum += other;
            }
        }
    }
    return sum;
}

enum class AliquotStatus {
    TERMINATING_ZERO,
    PERFECT_NUMBER,
    AMICABLE_CYCLE,
    SOCIABLE_CYCLE,
    EXCEEDED_STEP_LIMIT
};

struct AliquotResult {
    uint64_t startValue;
    std::vector<uint64_t> sequence;
    AliquotStatus status;
    int cycleLength;
};

/**
 * @brief Traces and classifies the aliquot sequence for a given start value.
 */
AliquotResult analyzeAliquot(uint64_t start, int maxSteps = 50) {
    std::vector<uint64_t> seq;
    std::unordered_map<uint64_t, int> visitedIndex;

    uint64_t current = start;
    seq.push_back(current);
    visitedIndex[current] = 0;

    for (int step = 1; step <= maxSteps; ++step) {
        uint64_t next = sumProperDivisors(current);
        seq.push_back(next);

        if (next == 0) {
            return {start, seq, AliquotStatus::TERMINATING_ZERO, 0};
        }

        if (visitedIndex.find(next) != visitedIndex.end()) {
            int cycleLen = step - visitedIndex[next];
            if (cycleLen == 1) {
                return {start, seq, AliquotStatus::PERFECT_NUMBER, 1};
            } else if (cycleLen == 2) {
                return {start, seq, AliquotStatus::AMICABLE_CYCLE, 2};
            } else {
                return {start, seq, AliquotStatus::SOCIABLE_CYCLE, cycleLen};
            }
        }

        visitedIndex[next] = step;
        current = next;

        // Prevent overflow on runaway sequences
        if (current > 100000000ULL) {
            break;
        }
    }

    return {start, seq, AliquotStatus::EXCEEDED_STEP_LIMIT, 0};
}

} // namespace number_theory

int main() {
    std::cout << "========================================\n";
    std::cout << "       ALIQUOT SEQUENCE ANALYZER        \n";
    std::cout << "========================================\n\n";

    std::vector<uint64_t> testCases = {6, 12, 28, 220, 284, 1184, 1210, 30};

    for (uint64_t n : testCases) {
        auto res = number_theory::analyzeAliquot(n);

        std::cout << "Start n = " << n << ":\n";
        std::cout << "  Trajectory: ";
        for (size_t i = 0; i < res.sequence.size(); ++i) {
            std::cout << res.sequence[i] << (i + 1 == res.sequence.size() ? "" : " -> ");
            if ((i + 1) % 10 == 0 && i + 1 < res.sequence.size()) std::cout << "\n              ";
        }
        std::cout << "\n";

        std::cout << "  Classification: ";
        switch (res.status) {
            case number_theory::AliquotStatus::TERMINATING_ZERO:
                std::cout << "TERMINATING (Ends at 0 after " << res.sequence.size() - 1 << " steps)\n";
                break;
            case number_theory::AliquotStatus::PERFECT_NUMBER:
                std::cout << "PERFECT NUMBER (Fixed Point, Period 1)\n";
                break;
            case number_theory::AliquotStatus::AMICABLE_CYCLE:
                std::cout << "AMICABLE PAIR (Periodic Cycle, Period 2)\n";
                break;
            case number_theory::AliquotStatus::SOCIABLE_CYCLE:
                std::cout << "SOCIABLE CYCLE (Period " << res.cycleLength << ")\n";
                break;
            case number_theory::AliquotStatus::EXCEEDED_STEP_LIMIT:
                std::cout << "LONG / UNBOUNDED (Exceeded search limit)\n";
                break;
        }
        std::cout << "\n";
    }

    std::cout << "Program finished successfully.\n";
    return 0;
}
