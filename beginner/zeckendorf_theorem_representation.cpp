/**
 * @file zeckendorf_theorem_representation.cpp
 * @brief Zeckendorf's Theorem representation and Fibonacci coding in C++17.
 *
 * Zeckendorf's Theorem:
 * Every positive integer can be uniquely represented as the sum of one or more
 * distinct, non-consecutive Fibonacci numbers.
 *
 * Sequence definition:
 *   F_2 = 1, F_3 = 2, F_4 = 3, F_5 = 5, F_6 = 8, F_7 = 13, ...
 *   (F_1 = 1 is omitted to ensure uniqueness).
 *
 * Algorithm:
 *   - Precompute Fibonacci numbers up to target N.
 *   - Greedy selection: Repeatedly pick the largest Fibonacci number F_k <= remaining N,
 *     and subtract F_k from N until N reaches 0.
 *   - Non-consecutive guarantee: If F_k and F_{k-1} were both selected, their sum
 *     F_k + F_{k-1} = F_{k+1} would have been chosen instead by the greedy choice.
 *
 * Fibonacci Coding:
 *   The representation can be formatted as a bitstring b_2 b_3 ... b_k 1,
 *   where b_i = 1 if F_i is used, with a terminating '1' indicating the end of the codeword.
 *
 * Complexity:
 *   - Greedy representation: O(log(N)) time, O(log(N)) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdint>
#include <algorithm>

namespace combinatorics {

/**
 * @brief Generates Fibonacci numbers starting from F_2 = 1, F_3 = 2, ... up to limit.
 */
std::vector<uint64_t> generateFibonacciUpTo(uint64_t limit) {
    std::vector<uint64_t> fib = {1, 2}; // F_2 = 1, F_3 = 2
    while (true) {
        uint64_t next = fib.back() + fib[fib.size() - 2];
        if (next > limit) break;
        fib.push_back(next);
    }
    return fib;
}

struct ZeckendorfResult {
    uint64_t originalNumber;
    std::vector<uint64_t> fibonacciTerms; // Descending order
    std::string fibonacciCode;            // Bitstring representation
};

/**
 * @brief Computes the unique non-consecutive Zeckendorf decomposition of N.
 * @param n Positive integer (n >= 1).
 * @return ZeckendorfResult containing decomposed terms and binary code.
 */
ZeckendorfResult getZeckendorfRepresentation(uint64_t n) {
    if (n == 0) {
        return {0, {}, "0"};
    }

    auto fib = generateFibonacciUpTo(n);
    uint64_t rem = n;
    std::vector<uint64_t> terms;
    std::vector<int> usedIndices;

    for (int i = static_cast<int>(fib.size()) - 1; i >= 0; --i) {
        if (fib[i] <= rem) {
            terms.push_back(fib[i]);
            usedIndices.push_back(i);
            rem -= fib[i];
        }
    }

    // Build Fibonacci code: bits from F_2 up to largest used index, plus trailing '1'
    int maxIdx = usedIndices.empty() ? 0 : usedIndices.front();
    std::string code(maxIdx + 1, '0');
    for (int idx : usedIndices) {
        code[idx] = '1';
    }
    code += "1"; // Terminator for Fibonacci coding

    return {n, terms, code};
}

} // namespace combinatorics

int main() {
    std::cout << "========================================\n";
    std::cout << "   ZECKENDORF'S THEOREM REPRESENTATION  \n";
    std::cout << "========================================\n\n";

    std::vector<uint64_t> testValues = {1, 4, 10, 17, 30, 42, 64, 100, 144, 250};

    std::cout << std::left << std::setw(6)  << "N"
              << std::setw(32) << "Zeckendorf Sum"
              << std::setw(18) << "Fibonacci Code"
              << "Verification\n";
    std::cout << std::string(72, '-') << "\n";

    for (uint64_t val : testValues) {
        auto res = combinatorics::getZeckendorfRepresentation(val);

        // Format sum string: e.g. "21 + 8 + 1"
        std::string sumStr;
        uint64_t checkSum = 0;
        for (size_t i = 0; i < res.fibonacciTerms.size(); ++i) {
            checkSum += res.fibonacciTerms[i];
            sumStr += std::to_string(res.fibonacciTerms[i]);
            if (i + 1 < res.fibonacciTerms.size()) sumStr += " + ";
        }

        std::cout << std::left << std::setw(6)  << val
                  << std::setw(32) << sumStr
                  << std::setw(18) << res.fibonacciCode
                  << (checkSum == val ? "Exact Match" : "Mismatch") << "\n";
    }

    std::cout << "\nNotes:\n";
    std::cout << "  - No two consecutive Fibonacci numbers appear in any sum.\n";
    std::cout << "  - Fibonacci coding appends a final '1' as a self-delimiting prefix code.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
