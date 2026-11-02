/**
 * @file fascinating_number_checker.cpp
 * @brief Checks if a number is a Fascinating Number and finds fascinating numbers in a range.
 *
 * Definition:
 * A Fascinating Number is an integer with three or more digits such that when it is
 * multiplied by 2 and by 3, concatenating the original number, its product with 2,
 * and its product with 3 produces a string that contains all the digits from 1 to 9
 * exactly once (i.e., pandigital for digits 1-9, excluding zero).
 *
 * Examples:
 * - 192:
 *   192 * 1 = 192
 *   192 * 2 = 384
 *   192 * 3 = 576
 *   Concatenation: "192" + "384" + "576" = "192384576"
 *   Contains digits 1, 2, 3, 4, 5, 6, 7, 8, 9 each exactly once. -> FASCINATING!
 *
 * - 219:
 *   219 * 1 = 219, 219 * 2 = 438, 219 * 3 = 657
 *   Concatenation: "219438657" -> FASCINATING!
 *
 * - 273:
 *   273 * 1 = 273, 273 * 2 = 546, 273 * 3 = 819
 *   Concatenation: "273546819" -> FASCINATING!
 *
 * Time Complexity:
 * - Single check: O(1) (concatenated length is always ~9 digits)
 * - Range search up to N: O(N)
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

struct FascinatingCheckResult {
    bool isFascinating;
    std::string concatenated;
    std::string reason;
};

/**
 * @brief Checks if a number is a Fascinating number.
 * @param n Integer to check (must be >= 100).
 * @return FascinatingCheckResult.
 */
FascinatingCheckResult checkFascinating(long long n) {
    if (n < 100) {
        return {false, "", "Number must have at least 3 digits"};
    }

    std::string s1 = std::to_string(n);
    std::string s2 = std::to_string(n * 2);
    std::string s3 = std::to_string(n * 3);
    std::string concat = s1 + s2 + s3;

    if (concat.length() != 9) {
        return {false, concat, "Concatenated length is not 9 (length = " + std::to_string(concat.length()) + ")"};
    }

    std::vector<int> freq(10, 0);
    for (char c : concat) {
        freq[c - '0']++;
    }

    if (freq[0] > 0) {
        return {false, concat, "Contains zero '0'"};
    }

    for (int d = 1; d <= 9; ++d) {
        if (freq[d] != 1) {
            return {false, concat, "Digit " + std::to_string(d) + " appears " + std::to_string(freq[d]) + " times"};
        }
    }

    return {true, concat, "Valid: contains digits 1-9 exactly once"};
}

/**
 * @brief Finds all Fascinating numbers up to a specified upper bound.
 */
std::vector<long long> findFascinatingNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 100; i <= limit; ++i) {
        if (checkFascinating(i).isFascinating) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Fascinating Number Checker & Generator ===\n\n";

    // 1. Verify selected test candidates
    std::vector<long long> testCandidates = {192, 219, 273, 327, 400, 500, 672, 853};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(16) << "Concatenated"
              << std::setw(16) << "Fascinating?"
              << "   Details\n";
    std::cout << "  " << std::string(68, '-') << '\n';

    for (long long num : testCandidates) {
        auto res = checkFascinating(num);
        std::cout << "  " << std::setw(8) << num
                  << std::setw(16) << res.concatenated
                  << std::setw(16) << (res.isFascinating ? "YES" : "No")
                  << "   " << res.reason << '\n';
    }

    // 2. Find all 3-digit fascinating numbers
    std::cout << "\n2. All 3-digit Fascinating Numbers (100 to 999):\n  ";
    auto list = findFascinatingNumbers(999);
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << list[i];
        if (i + 1 < list.size()) std::cout << ", ";
    }
    std::cout << "\n\nTotal 3-digit Fascinating numbers found: " << list.size() << '\n';

    return 0;
}
