/*
 * Program Description:
 * Perfect Square Checker using Integer Binary Search in C++.
 * Determines whether a given 64-bit integer N is a perfect square
 * without using floating-point math (e.g. std::sqrt), avoiding precision pitfalls.
 *
 * Algorithm Details:
 * Binary search range [1, N]:
 * mid = left + (right - left) / 2
 * - If mid * mid == N: Return true.
 * - If mid * mid < N: Search right half (left = mid + 1).
 * - If mid * mid > N (or mid > N / mid to prevent overflow): Search left half (right = mid - 1).
 *
 * Time Complexity: O(log N)
 * Space Complexity: O(1).
 */

#include <iostream>
#include <vector>
#include <optional>

// Determines if n is a perfect square and returns its integer square root
std::optional<unsigned long long> integerSquareRoot(unsigned long long n) {
    if (n == 0 || n == 1) return n;

    unsigned long long left = 1, right = n / 2;
    while (left <= right) {
        unsigned long long mid = left + (right - left) / 2;

        // Use division to prevent 64-bit multiplication overflow
        if (mid == n / mid && (n % mid == 0)) {
            return mid;
        } else if (mid <= n / mid) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return std::nullopt;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Perfect Square Checker (Binary Search)\n";
    std::cout << "==========================================\n\n";

    std::vector<unsigned long long> testNumbers = {
        0, 1, 4, 14, 16, 25, 26, 144, 256, 1000, 1000000, 1000000000000ULL, 1000000000001ULL
    };

    for (unsigned long long n : testNumbers) {
        auto root = integerSquareRoot(n);
        if (root) {
            std::cout << "Number " << n << " -> PERFECT SQUARE ("
                      << *root << "^2 = " << n << ") [YES]\n";
        } else {
            std::cout << "Number " << n << " -> NOT a perfect square [NO]\n";
        }
    }

    return 0;
}
