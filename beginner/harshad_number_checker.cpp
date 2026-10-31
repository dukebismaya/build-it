/**
 * @file harshad_number_checker.cpp
 * @brief Checks if a number is a Harshad (Niven) number and finds Harshad numbers in a range.
 *
 * Definition:
 * A Harshad number (also known as a Niven number, named by Ivan M. Niven in 1977)
 * is an integer that is divisible by the sum of its digits when written in a given base
 * (typically base 10).
 *
 * Examples:
 * - 18:   1 + 8 = 9      -> 18 % 9 == 0 (Harshad, quotient = 2)
 * - 21:   2 + 1 = 3      -> 21 % 3 == 0 (Harshad, quotient = 7)
 * - 1729: 1 + 7 + 2 + 9 = 19 -> 1729 % 19 == 0 (Harshad, Ramanujan-Hardy number, quotient = 91)
 * - 19:   1 + 9 = 10     -> 19 % 10 != 0 (Not Harshad)
 *
 * Time Complexity:
 * - Single check: O(log10(N))
 * - Range search up to N: O(N * log10(N))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

/**
 * @brief Computes the sum of digits of a non-negative integer in base 10.
 * @param n Integer value.
 * @return Sum of base-10 digits.
 */
long long sumOfDigits(long long n) {
    long long sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

/**
 * @brief Checks whether a given positive integer is a Harshad number.
 * @param n Integer to check.
 * @return True if n is divisible by its sum of digits, false otherwise.
 */
bool isHarshad(long long n) {
    if (n <= 0) return false;
    long long sum = sumOfDigits(n);
    return (n % sum) == 0;
}

/**
 * @brief Finds all Harshad numbers up to a specified upper bound.
 * @param limit Upper bound inclusive.
 * @return Vector of Harshad numbers.
 */
std::vector<long long> findHarshadNumbers(long long limit) {
    std::vector<long long> results;
    for (long long i = 1; i <= limit; ++i) {
        if (isHarshad(i)) {
            results.push_back(i);
        }
    }
    return results;
}

int main() {
    std::cout << "=== Harshad (Niven) Number Checker & Generator ===\n\n";

    // 1. Check selected candidates
    std::vector<long long> testNumbers = {18, 19, 21, 24, 27, 45, 100, 1729, 2024, 2026};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(10) << "Number"
              << std::setw(16) << "Sum of Digits"
              << std::setw(16) << "Is Harshad?"
              << std::setw(16) << "Quotient (N/Sum)" << '\n';
    std::cout << "  " << std::string(58, '-') << '\n';

    for (long long num : testNumbers) {
        long long sum = sumOfDigits(num);
        bool harshad = isHarshad(num);
        std::cout << "  " << std::setw(10) << num
                  << std::setw(16) << sum
                  << std::setw(16) << (harshad ? "YES" : "No");
        if (harshad) {
            std::cout << std::setw(16) << (num / sum) << '\n';
        } else {
            std::cout << std::setw(16) << "-" << '\n';
        }
    }

    // 2. Range search up to 100
    std::cout << "\n2. All Harshad Numbers up to 100:\n  ";
    auto list = findHarshadNumbers(100);
    for (size_t i = 0; i < list.size(); ++i) {
        std::cout << list[i];
        if (i + 1 < list.size()) std::cout << ", ";
    }
    std::cout << "\n\nTotal Harshad numbers in [1, 100]: " << list.size() << '\n';

    return 0;
}
