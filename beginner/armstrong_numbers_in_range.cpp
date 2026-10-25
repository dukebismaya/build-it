/*
 * Program Description:
 * Armstrong Numbers in Range Finder in C++.
 * An Armstrong (Narcissistic / Pluperfect digital invariant) number of order K
 * (where K is the number of digits) is a number that is equal to the sum of its
 * digits each raised to the power of K.
 *
 * Examples:
 * - Order 1: 1..9 (e.g. 7^1 = 7)
 * - Order 3: 153 = 1^3 + 5^3 + 3^3 = 1 + 125 + 27 = 153
 * - Order 3: 370 = 3^3 + 7^3 + 0^3 = 27 + 343 + 0 = 370
 * - Order 4: 1634 = 1^4 + 6^4 + 3^4 + 4^4 = 1 + 1296 + 81 + 256 = 1634
 *
 * Time Complexity: O((R - L + 1) * log10(R))
 * Space Complexity: O(1).
 */

#include <iostream>
#include <vector>
#include <cmath>

// Computes integer power base^exp
long long intPower(int base, int exp) {
    long long res = 1;
    for (int i = 0; i < exp; ++i) {
        res *= base;
    }
    return res;
}

// Checks if a given number is an Armstrong number
bool isArmstrong(long long n) {
    if (n < 0) return false;

    // Count digits
    long long temp = n;
    int k = 0;
    while (temp > 0) {
        k++;
        temp /= 10;
    }

    if (k == 0) return true; // 0 is an Armstrong number of order 1

    temp = n;
    long long sum = 0;
    while (temp > 0) {
        int digit = temp % 10;
        sum += intPower(digit, k);
        temp /= 10;
    }

    return sum == n;
}

// Finds all Armstrong numbers within interval [low, high]
std::vector<long long> findArmstrongInRange(long long low, long long high) {
    std::vector<long long> result;
    for (long long num = low; num <= high; ++num) {
        if (isArmstrong(num)) {
            result.push_back(num);
        }
    }
    return result;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Armstrong Numbers in Range Finder    \n";
    std::cout << "==========================================\n\n";

    long long low = 1, high = 10000;
    std::cout << "Finding all Armstrong numbers in range [" << low << " .. " << high << "]:\n";

    auto armstrongs = findArmstrongInRange(low, high);

    std::cout << "Total found: " << armstrongs.size() << " numbers\n\n";
    for (size_t i = 0; i < armstrongs.size(); ++i) {
        std::cout << armstrongs[i] << (i + 1 < armstrongs.size() ? ", " : "\n");
    }

    std::cout << "\nBreakdown:\n";
    std::cout << "  1-digit: 1, 2, 3, 4, 5, 6, 7, 8, 9\n";
    std::cout << "  3-digit: 153, 370, 371, 407\n";
    std::cout << "  4-digit: 1634, 8208, 9474\n";

    return 0;
}
