/**
 * @file keith_number_checker.cpp
 * @brief Checks if a number is a Keith (repfigit) Number and finds Keith numbers in a range.
 *
 * Definition:
 * A Keith Number (also called a repfigit, for "repetitive Fibonacci-like digit")
 * is an integer N with d digits (d >= 2) that appears in a sequence where the first
 * d terms are the decimal digits of N, and each subsequent term is the sum of the
 * preceding d terms.
 *
 * Examples:
 * - 14:
 *   Digits: 1, 4 (d = 2)
 *   Sequence: 1, 4, (1+4=5), (4+5=9), (5+9=14)
 *   14 appears in the sequence! -> KEITH NUMBER
 *
 * - 19:
 *   Digits: 1, 9 (d = 2)
 *   Sequence: 1, 9, (1+9=10), (9+10=19)
 *   19 appears in the sequence! -> KEITH NUMBER
 *
 * - 197:
 *   Digits: 1, 9, 7 (d = 3)
 *   Sequence: 1, 9, 7, (1+9+7=17), (9+7+17=33), (7+17+33=57), (17+33+57=107), (33+57+107=197)
 *   197 appears in the sequence! -> KEITH NUMBER
 *
 * - 742:
 *   Digits: 7, 4, 2 (d = 3)
 *   Sequence reaches 742! -> KEITH NUMBER
 *
 * Time Complexity:
 * - Single check: O(k * d) where k is the number of steps until term >= N (typically k < 30)
 * - Space Complexity: O(d) for sliding window
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include <string>

struct KeithResult {
    bool isKeith;
    std::vector<long long> sequence;
};

/**
 * @brief Checks if a positive integer N is a Keith Number.
 * @param n Number to check (must be >= 10).
 * @return KeithResult containing status and generated sequence.
 */
KeithResult checkKeithNumber(long long n) {
    if (n < 10) return {false, {}};

    std::string s = std::to_string(n);
    int d = static_cast<int>(s.length());

    std::vector<long long> seq;
    for (char c : s) {
        seq.push_back(c - '0');
    }

    while (true) {
        long long nextTerm = 0;
        int sz = static_cast<int>(seq.size());
        for (int i = sz - d; i < sz; ++i) {
            nextTerm += seq[i];
        }

        seq.push_back(nextTerm);

        if (nextTerm == n) {
            return {true, seq};
        }
        if (nextTerm > n) {
            return {false, seq};
        }
    }
}

int main() {
    std::cout << "=== Keith (Repfigit) Number Checker ===\n\n";

    // 1. Evaluate selected candidates
    std::vector<long long> testCandidates = {14, 19, 28, 47, 61, 75, 197, 742, 1000};

    std::cout << "1. Verification of Selected Candidates:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(14) << "Is Keith?"
              << "   Generated Sequence\n";
    std::cout << "  " << std::string(64, '-') << '\n';

    for (long long num : testCandidates) {
        auto res = checkKeithNumber(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(14) << (res.isKeith ? "YES" : "No") << "   [ ";

        for (size_t i = 0; i < res.sequence.size(); ++i) {
            std::cout << res.sequence[i];
            if (i + 1 < res.sequence.size()) std::cout << ", ";
        }
        std::cout << " ]\n";
    }

    // 2. Find all Keith numbers up to 1,000
    std::cout << "\n2. All Keith Numbers up to 1,000:\n  ";
    std::vector<long long> keithList;
    for (long long i = 10; i <= 1000; ++i) {
        if (checkKeithNumber(i).isKeith) {
            keithList.push_back(i);
            std::cout << i << " ";
        }
    }
    std::cout << "\n\nTotal found in [10, 1000]: " << keithList.size() << '\n';

    return 0;
}

