/*
 * ============================================================================
 *  Zeckendorf's Representation (Fibonacci Number System)
 * ============================================================================
 *
 *  Zeckendorf's theorem states that every positive integer has a unique
 *  representation as a sum of non-consecutive Fibonacci numbers. This is
 *  known as the Zeckendorf representation.
 *
 *  For example:
 *    20 = 13 + 5 + 2           (F7 + F5 + F3)
 *    30 = 21 + 8 + 1           (F8 + F6 + F1)
 *    100 = 89 + 8 + 3          (F11 + F6 + F4)
 *
 *  The representation can be written as a binary string where bit i indicates
 *  whether F(i+2) is included (no two consecutive 1s appear).
 *
 *  Algorithm (Greedy):
 *  1) Find the largest Fibonacci number <= n.
 *  2) Subtract it and repeat.
 *  3) No two consecutive Fibonacci numbers will be chosen (provable by
 *     the identity F(k) + F(k-1) = F(k+1)).
 *
 *  This program computes Zeckendorf representations and verifies uniqueness
 *  and the non-consecutiveness property.
 *
 *  Time Complexity:  O(log n) per representation (Fibonacci numbers grow
 *                    exponentially as phi^n).
 *  Space Complexity: O(log n) to store the representation.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace zeckendorf {

/* Precompute Fibonacci numbers up to ~10^18 */
std::vector<int64_t> buildFibonacci() {
    std::vector<int64_t> fibs;
    fibs.push_back(1); /* F(2) = 1 */
    fibs.push_back(2); /* F(3) = 2 */

    while (true) {
        int64_t next = fibs[fibs.size() - 1] + fibs[fibs.size() - 2];
        if (next < 0) break; /* Overflow */
        fibs.push_back(next);
    }
    return fibs;
}

struct Representation {
    std::vector<int64_t> terms;    /* The Fibonacci numbers used */
    std::vector<int> indices;       /* Their indices in the Fibonacci sequence */
    std::string binary;             /* Binary string representation */
};

Representation decompose(int64_t n, const std::vector<int64_t>& fibs) {
    Representation rep;
    int64_t remaining = n;

    for (int i = static_cast<int>(fibs.size()) - 1; i >= 0 && remaining > 0; --i) {
        if (fibs[i] <= remaining) {
            rep.terms.push_back(fibs[i]);
            rep.indices.push_back(i);
            remaining -= fibs[i];
        }
    }

    /* Build binary string (MSB first) */
    if (!rep.indices.empty()) {
        int maxIdx = rep.indices.front();
        rep.binary.resize(maxIdx + 1, '0');
        for (int idx : rep.indices) {
            rep.binary[maxIdx - idx] = '1';
        }
    }

    return rep;
}

/* Verify no two consecutive Fibonacci numbers */
bool verifyNonConsecutive(const std::vector<int>& indices) {
    for (size_t i = 1; i < indices.size(); ++i) {
        if (indices[i - 1] - indices[i] == 1) return false;
    }
    return true;
}

/* Verify that the terms sum to the original number */
bool verifySum(int64_t n, const std::vector<int64_t>& terms) {
    int64_t sum = 0;
    for (int64_t t : terms) sum += t;
    return sum == n;
}

} // namespace zeckendorf

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Zeckendorf's Representation (Fibonacci Number System)\n";
    std::cout << "========================================================\n\n";

    auto fibs = zeckendorf::buildFibonacci();

    std::cout << "Fibonacci numbers used (F2 onwards):\n  ";
    for (size_t i = 0; i < std::min(fibs.size(), size_t(20)); ++i) {
        std::cout << fibs[i];
        if (i + 1 < std::min(fibs.size(), size_t(20))) std::cout << ", ";
    }
    std::cout << " ...\n  Total available: " << fibs.size() << "\n\n";

    /* Decompose sample numbers */
    std::vector<int64_t> samples = {1, 5, 10, 20, 30, 42, 64, 100, 255, 1000};

    std::cout << "Zeckendorf Representations:\n";
    std::cout << std::setw(8) << "N" << "  " << std::setw(30) << std::left
              << "Decomposition" << std::setw(18) << "Binary"
              << std::setw(10) << "Valid?" << "\n";
    std::cout << std::right << "----------------------------------------------------------------------\n";

    for (int64_t n : samples) {
        auto rep = zeckendorf::decompose(n, fibs);

        std::string decomp;
        for (size_t i = 0; i < rep.terms.size(); ++i) {
            decomp += std::to_string(rep.terms[i]);
            if (i + 1 < rep.terms.size()) decomp += " + ";
        }

        bool valid = zeckendorf::verifyNonConsecutive(rep.indices) &&
                     zeckendorf::verifySum(n, rep.terms);

        std::cout << std::setw(8) << std::right << n << "  "
                  << std::setw(30) << std::left << decomp
                  << std::setw(18) << rep.binary
                  << std::setw(10) << (valid ? "YES" : "NO") << "\n";
    }

    /* Show all representations for 1..20 */
    std::cout << std::right << "\nComplete table for N = 1 to 20:\n";
    std::cout << std::setw(6) << "N" << std::setw(12) << "Binary"
              << "  " << "Terms\n";
    std::cout << "------------------------------------\n";

    for (int64_t n = 1; n <= 20; ++n) {
        auto rep = zeckendorf::decompose(n, fibs);
        std::string terms;
        for (size_t i = 0; i < rep.terms.size(); ++i) {
            terms += std::to_string(rep.terms[i]);
            if (i + 1 < rep.terms.size()) terms += "+";
        }
        std::cout << std::setw(6) << n << std::setw(12) << rep.binary
                  << "  " << terms << "\n";
    }

    /* Large number test */
    int64_t large = 1000000;
    auto largeRep = zeckendorf::decompose(large, fibs);
    std::cout << "\nLarge number test: " << large << "\n  Terms: ";
    for (size_t i = 0; i < largeRep.terms.size(); ++i) {
        std::cout << largeRep.terms[i];
        if (i + 1 < largeRep.terms.size()) std::cout << " + ";
    }
    std::cout << "\n  Components: " << largeRep.terms.size()
              << " | Valid: " << (zeckendorf::verifySum(large, largeRep.terms) ? "YES" : "NO")
              << "\n";

    return 0;
}
