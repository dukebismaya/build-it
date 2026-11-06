/**
 * @file evil_odious_number_checker.cpp
 * @brief Classifies numbers as Evil or Odious based on their binary Hamming weight.
 *
 * Definition:
 * - Evil Number:   A non-negative integer that has an EVEN number of 1's in its binary expansion.
 * - Odious Number: A non-negative integer that has an ODD number of 1's in its binary expansion.
 *
 * Connection to the Thue-Morse Sequence:
 * The n-th term of the Thue-Morse sequence is t_n = (count of 1s in binary n) % 2.
 * Thus:
 *   t_n == 0  <=>  n is an EVIL number.
 *   t_n == 1  <=>  n is an ODIOUS number.
 *
 * First Few Evil Numbers:   0, 3, 5, 6, 9, 10, 12, 15, 17, 18, 20, 23, 24, 27, 29, 30
 * First Few Odious Numbers: 1, 2, 4, 7, 8, 11, 13, 14, 16, 19, 21, 22, 25, 26, 28, 31
 *
 * Time Complexity: O(log2(N)) via popcount
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <bitset>
#include <algorithm>
#include <iomanip>
#include <string>

/**
 * @brief Computes the number of set bits (1s) in the binary representation of n.
 */
int countSetBits(unsigned long long n) {
    int count = 0;
    while (n > 0) {
        n &= (n - 1); // Brian Kernighan's bit-counting algorithm
        count++;
    }
    return count;
}

/**
 * @brief Checks if n is an Evil number.
 */
bool isEvil(unsigned long long n) {
    return (countSetBits(n) % 2) == 0;
}

/**
 * @brief Converts a number to its minimal binary string representation.
 */
std::string toBinaryString(unsigned long long n) {
    if (n == 0) return "0";
    std::string bin;
    while (n > 0) {
        bin.push_back((n & 1) ? '1' : '0');
        n >>= 1;
    }
    std::reverse(bin.begin(), bin.end());
    return bin;
}

int main() {
    std::cout << "=== Evil and Odious Number Classifier ===\n\n";

    // 1. Evaluate numbers from 0 to 15
    std::cout << "1. Classification of Integers from 0 to 15:\n";
    std::cout << "  " << std::setw(6) << "Decimal"
              << std::setw(12) << "Binary"
              << std::setw(14) << "Set Bits (1s)"
              << std::setw(14) << "Classification" << '\n';
    std::cout << "  " << std::string(46, '-') << '\n';

    for (unsigned long long i = 0; i <= 15; ++i) {
        int bits = countSetBits(i);
        bool evil = isEvil(i);
        std::cout << "  " << std::setw(6) << i
                  << std::setw(12) << toBinaryString(i)
                  << std::setw(14) << bits
                  << std::setw(14) << (evil ? "EVIL" : "ODIOUS") << '\n';
    }

    // 2. Separate range [0, 30] into lists
    std::cout << "\n2. Evil Numbers in [0, 30]:\n  ";
    for (unsigned long long i = 0; i <= 30; ++i) {
        if (isEvil(i)) std::cout << i << " ";
    }

    std::cout << "\n\n3. Odious Numbers in [0, 30]:\n  ";
    for (unsigned long long i = 0; i <= 30; ++i) {
        if (!isEvil(i)) std::cout << i << " ";
    }
    std::cout << '\n';

    return 0;
}
