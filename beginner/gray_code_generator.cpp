/*
 * Program Description:
 * Gray Code (Reflected Binary Code) Generator and Decoder in C++.
 * Gray code is a binary numeral system where two successive values differ in only one bit position.
 *
 * Mathematical formulas:
 * 1. Binary to Gray Code:
 *    G(n) = n ^ (n >> 1)
 * 2. Gray Code to Binary:
 *    b = g ^ (g >> 1) ^ (g >> 2) ^ ...
 *
 * Features:
 * - Generates 2^N Gray code sequence for any N.
 * - Formatted binary bitstring display.
 * - Verifies that adjacent elements differ by a Hamming distance of exactly 1.
 */

#include <iostream>
#include <vector>
#include <bitset>
#include <iomanip>

// Encodes integer n to Gray code
unsigned int binaryToGray(unsigned int n) {
    return n ^ (n >> 1);
}

// Decodes Gray code g back to standard binary integer
unsigned int grayToBinary(unsigned int g) {
    unsigned int n = 0;
    for (; g; g >>= 1) {
        n ^= g;
    }
    return n;
}

// Generates the complete sequence of N-bit Gray codes
std::vector<unsigned int> generateGrayCodeSequence(int n) {
    std::vector<unsigned int> sequence;
    int total = 1 << n;
    sequence.reserve(total);

    for (int i = 0; i < total; ++i) {
        sequence.push_back(binaryToGray(i));
    }

    return sequence;
}

// Counts bit differences between two numbers (Hamming distance)
int hammingDistance(unsigned int a, unsigned int b) {
    unsigned int diff = a ^ b;
    int count = 0;
    while (diff > 0) {
        count += (diff & 1);
        diff >>= 1;
    }
    return count;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Gray Code Generator & Decoder     \n";
    std::cout << "==========================================\n\n";

    int n = 4; // 4-bit Gray code sequence
    std::cout << "Generating " << n << "-bit Gray Code sequence (2^" << n << " = " << (1 << n) << " values):\n\n";

    auto sequence = generateGrayCodeSequence(n);

    std::cout << "Idx | Binary (n) | Gray Code | Gray (Bits) | Decoded\n";
    std::cout << "----+------------+-----------+-------------+--------\n";

    bool allAdjacentValid = true;

    for (size_t i = 0; i < sequence.size(); ++i) {
        unsigned int g = sequence[i];
        unsigned int decoded = grayToBinary(g);

        std::bitset<4> bits(g);

        std::cout << std::setw(3) << i << " | "
                  << std::setw(10) << i << " | "
                  << std::setw(9) << g << " | "
                  << std::setw(11) << bits << " | "
                  << std::setw(7) << decoded << "\n";

        if (i > 0) {
            if (hammingDistance(sequence[i - 1], sequence[i]) != 1) {
                allAdjacentValid = false;
            }
        }
    }

    // Cyclic check: last and first element should also differ by 1 bit
    if (hammingDistance(sequence.front(), sequence.back()) != 1) {
        allAdjacentValid = false;
    }

    std::cout << "\nAdjacent & Cyclic Single-Bit Flip Check: "
              << (allAdjacentValid ? "PASSED [100%]" : "FAILED") << "\n";

    return 0;
}
