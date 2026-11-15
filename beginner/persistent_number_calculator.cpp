/*
 * ============================================================================
 *  Multiplicative Persistence & Digital Root Calculator
 * ============================================================================
 *
 *  Multiplicative Persistence:
 *    The number of times you must multiply the digits of a number together
 *    until you reach a single digit. For example:
 *      277 -> 2*7*7 = 98 -> 9*8 = 72 -> 7*2 = 14 -> 1*4 = 4  (persistence = 4)
 *
 *  The smallest number with persistence 11 is 277777788888899 (found by
 *  computer search). No number with persistence >= 12 has been found, and
 *  it is conjectured that 11 is the maximum.
 *
 *  Additive Persistence:
 *    Same idea but using digit sums:
 *      9999 -> 9+9+9+9 = 36 -> 3+6 = 9  (additive persistence = 2)
 *
 *  Digital Root (additive):
 *    The single digit you end up with. It equals n mod 9 (with 9 mapping to 9).
 *    dr(n) = 1 + ((n - 1) mod 9)  for n > 0
 *
 *  Time Complexity:  O(d * p) where d = digits, p = persistence
 *  Space Complexity: O(d) for storing intermediate digit sequences
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>

namespace persistence {

struct PersistenceResult {
    std::vector<int64_t> chain;
    int steps;
};

std::vector<int> getDigits(int64_t n) {
    if (n == 0) return {0};
    std::vector<int> digits;
    while (n > 0) {
        digits.push_back(static_cast<int>(n % 10));
        n /= 10;
    }
    return digits;
}

PersistenceResult multiplicativePersistence(int64_t n) {
    PersistenceResult result;
    result.chain.push_back(n);
    result.steps = 0;

    int64_t current = n;
    while (current >= 10) {
        auto digits = getDigits(current);
        int64_t product = 1;
        for (int d : digits) {
            product *= d;
        }
        current = product;
        result.chain.push_back(current);
        result.steps++;
    }

    return result;
}

PersistenceResult additivePersistence(int64_t n) {
    PersistenceResult result;
    result.chain.push_back(n);
    result.steps = 0;

    int64_t current = n;
    while (current >= 10) {
        auto digits = getDigits(current);
        int64_t sum = 0;
        for (int d : digits) {
            sum += d;
        }
        current = sum;
        result.chain.push_back(current);
        result.steps++;
    }

    return result;
}

int digitalRoot(int64_t n) {
    if (n == 0) return 0;
    int64_t rem = n % 9;
    return (rem == 0) ? 9 : static_cast<int>(rem);
}

int multiplicativeDigitalRoot(int64_t n) {
    while (n >= 10) {
        auto digits = getDigits(n);
        int64_t product = 1;
        for (int d : digits) {
            product *= d;
        }
        n = product;
    }
    return static_cast<int>(n);
}

} // namespace persistence

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Multiplicative Persistence & Digital Root Calculator\n";
    std::cout << "========================================================\n\n";

    /* Multiplicative persistence examples */
    std::vector<int64_t> examples = {10, 25, 39, 77, 277, 679, 6788, 68889};

    std::cout << "Multiplicative Persistence Chains:\n";
    std::cout << "--------------------------------------------\n";

    for (int64_t n : examples) {
        auto result = persistence::multiplicativePersistence(n);
        std::cout << "  " << std::setw(8) << n << " (persistence " << result.steps << "): ";
        for (size_t i = 0; i < result.chain.size(); ++i) {
            std::cout << result.chain[i];
            if (i + 1 < result.chain.size()) std::cout << " -> ";
        }
        std::cout << "\n";
    }

    /* Additive persistence examples */
    std::cout << "\nAdditive Persistence Chains:\n";
    std::cout << "--------------------------------------------\n";

    std::vector<int64_t> addExamples = {199, 9999, 123456789, 99999999};
    for (int64_t n : addExamples) {
        auto result = persistence::additivePersistence(n);
        std::cout << "  " << std::setw(12) << n << " (persistence " << result.steps << "): ";
        for (size_t i = 0; i < result.chain.size(); ++i) {
            std::cout << result.chain[i];
            if (i + 1 < result.chain.size()) std::cout << " -> ";
        }
        std::cout << "\n";
    }

    /* Digital root verification */
    std::cout << "\nDigital Root Verification (formula vs iterative):\n";
    std::cout << std::setw(12) << "Number" << std::setw(14) << "Add. Root"
              << std::setw(14) << "Mult. Root" << "\n";
    std::cout << "----------------------------------------\n";

    std::vector<int64_t> drExamples = {0, 1, 9, 18, 123, 9999, 493827};
    for (int64_t n : drExamples) {
        int addRoot = persistence::digitalRoot(n);
        int mulRoot = persistence::multiplicativeDigitalRoot(n);
        std::cout << std::setw(12) << n << std::setw(14) << addRoot
                  << std::setw(14) << mulRoot << "\n";
    }

    /* Survey: highest multiplicative persistence in [1, 10000] */
    int maxPers = 0;
    int64_t maxPersNum = 0;
    for (int64_t n = 1; n <= 10000; ++n) {
        auto result = persistence::multiplicativePersistence(n);
        if (result.steps > maxPers) {
            maxPers = result.steps;
            maxPersNum = n;
        }
    }
    std::cout << "\nHighest multiplicative persistence in [1, 10000]: "
              << maxPersNum << " with persistence " << maxPers << "\n";

    return 0;
}
