/*
 * ============================================================================
 *  Narcissistic (Armstrong / Pluperfect Digital Invariant) Number Checker
 * ============================================================================
 *
 *  A narcissistic number (also called a pluperfect digital invariant or PPDI)
 *  in base 10 is a number of d digits where the sum of each digit raised to
 *  the d-th power equals the number itself:
 *
 *    N = sum_{i=0}^{d-1} (digit_i)^d
 *
 *  Examples:
 *    153 = 1^3 + 5^3 + 3^3
 *    370 = 3^3 + 7^3 + 0^3
 *    8208 = 8^4 + 2^4 + 0^4 + 8^4
 *
 *  Note: The standard Armstrong number checks d=3 only. This program uses the
 *  generalized definition (pluperfect digital invariant) for any digit count.
 *
 *  This program checks individual numbers, then finds all narcissistic numbers
 *  up to a given limit.
 *
 *  Time Complexity: O(limit * d) where d = number of digits
 *  Space Complexity: O(1) auxiliary
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <iomanip>

namespace narcissistic {

int countDigits(int64_t n) {
    if (n == 0) return 1;
    int count = 0;
    int64_t temp = n;
    while (temp > 0) {
        ++count;
        temp /= 10;
    }
    return count;
}

int64_t power(int64_t base, int exp) {
    int64_t result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}

bool isNarcissistic(int64_t n) {
    if (n < 0) return false;
    int d = countDigits(n);
    int64_t sum = 0;
    int64_t temp = n;

    while (temp > 0) {
        int digit = static_cast<int>(temp % 10);
        sum += power(digit, d);
        temp /= 10;

        /* Early exit: sum already exceeded n */
        if (sum > n) return false;
    }

    return sum == n;
}

std::vector<int64_t> findAllUpTo(int64_t limit) {
    std::vector<int64_t> results;
    for (int64_t n = 0; n <= limit; ++n) {
        if (isNarcissistic(n)) {
            results.push_back(n);
        }
    }
    return results;
}

void showDecomposition(int64_t n) {
    int d = countDigits(n);
    std::vector<int> digits;
    int64_t temp = n;
    while (temp > 0) {
        digits.push_back(static_cast<int>(temp % 10));
        temp /= 10;
    }

    std::cout << n << " = ";
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        std::cout << digits[i] << "^" << d;
        if (i > 0) std::cout << " + ";
    }

    int64_t sum = 0;
    for (int digit : digits) {
        sum += power(digit, d);
    }
    std::cout << " = " << sum << (sum == n ? " (verified)" : " (error)") << "\n";
}

} // namespace narcissistic

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Narcissistic (Pluperfect Digital Invariant) Number Checker\n";
    std::cout << "========================================================\n\n";

    /* Check specific numbers */
    std::vector<int64_t> testValues = {1, 7, 153, 370, 371, 407, 1634, 8208, 9474, 100, 999};
    std::cout << "Individual checks:\n";
    std::cout << std::setw(10) << "Number" << std::setw(18) << "Narcissistic?" << "\n";
    std::cout << "----------------------------\n";

    for (int64_t v : testValues) {
        std::cout << std::setw(10) << v
                  << std::setw(18) << (narcissistic::isNarcissistic(v) ? "YES" : "NO")
                  << "\n";
    }

    /* Show decomposition of known narcissistic numbers */
    std::cout << "\nDecomposition of narcissistic numbers:\n";
    narcissistic::showDecomposition(153);
    narcissistic::showDecomposition(370);
    narcissistic::showDecomposition(371);
    narcissistic::showDecomposition(407);
    narcissistic::showDecomposition(1634);
    narcissistic::showDecomposition(8208);
    narcissistic::showDecomposition(9474);

    /* Find all narcissistic numbers up to 1,000,000 */
    std::cout << "\nAll narcissistic numbers up to 1,000,000:\n  ";
    auto all = narcissistic::findAllUpTo(1000000);
    for (size_t i = 0; i < all.size(); ++i) {
        std::cout << all[i];
        if (i + 1 < all.size()) std::cout << ", ";
    }
    std::cout << "\n  Total found: " << all.size() << "\n";

    return 0;
}
