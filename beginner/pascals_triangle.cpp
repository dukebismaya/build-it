/*
 * Program Description:
 * Generates Pascal's Triangle up to N rows.
 * Each number in Pascal's Triangle is the sum of the two directly above it.
 * It also corresponds to the binomial coefficient C(n, k) = n! / (k! * (n-k)!).
 *
 * Concepts covered:
 * - 2D dynamic vector generation
 * - Binomial coefficient formula calculation
 * - Pretty-printed center-aligned pyramid formatting
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

// Generates Pascal's triangle using iterative dynamic programming (row-by-row)
std::vector<std::vector<long long>> generatePascalsTriangle(int numRows) {
    std::vector<std::vector<long long>> triangle;
    if (numRows <= 0) return triangle;

    triangle.reserve(numRows);
    for (int i = 0; i < numRows; ++i) {
        std::vector<long long> row(i + 1, 1);
        for (int j = 1; j < i; ++j) {
            row[j] = triangle[i - 1][j - 1] + triangle[i - 1][j];
        }
        triangle.push_back(row);
    }
    return triangle;
}

// Computes combination C(n, k) directly in O(k) time without overflow
long long nCr(int n, int r) {
    if (r < 0 || r > n) return 0;
    if (r == 0 || r == n) return 1;
    if (r > n - r) r = n - r; // Symmetry property C(n, r) = C(n, n-r)

    long long result = 1;
    for (int i = 1; i <= r; ++i) {
        result = result * (n - i + 1) / i;
    }
    return result;
}

// Pretty prints Pascal's Triangle as a centered pyramid
void printPascalsTrianglePyramid(const std::vector<std::vector<long long>>& triangle) {
    int rows = static_cast<int>(triangle.size());
    const int fieldWidth = 6;

    for (int i = 0; i < rows; ++i) {
        // Leading space for centered pyramid alignment
        int leadingSpaces = (rows - i - 1) * (fieldWidth / 2);
        std::cout << std::string(leadingSpaces, ' ');

        for (int j = 0; j <= i; ++j) {
            std::cout << std::setw(fieldWidth) << triangle[i][j];
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Pascal's Triangle Generator       \n";
    std::cout << "==========================================\n\n";

    int n = 8;
    std::cout << "Generating Pascal's Triangle for " << n << " rows:\n\n";
    auto triangle = generatePascalsTriangle(n);
    printPascalsTrianglePyramid(triangle);

    std::cout << "\n--- Verification using Combinatorial Formula C(n, r) ---\n";
    std::cout << "Row 5 items (C(5, r) for r = 0..5):\n";
    for (int r = 0; r <= 5; ++r) {
        std::cout << "C(5, " << r << ") = " << nCr(5, r) << "\n";
    }

    std::cout << "\nSum of elements in row 5: ";
    long long rowSum = 0;
    for (long long val : triangle[5]) rowSum += val;
    std::cout << rowSum << " (Expected 2^5 = " << (1LL << 5) << ")\n";

    return 0;
}
