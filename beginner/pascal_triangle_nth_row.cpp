/*
 * Program Description:
 * Pascal's Triangle N-th Row Generator in C++.
 * Given an integer rowIndex N (0-indexed), returns only the N-th row of Pascal's Triangle
 * in O(N) time and O(N) space (O(1) extra space).
 *
 * Mathematical Identity:
 * The k-th element of row N is C(N, k).
 * Rather than computing factorials or full 2D tables, each successive term is derived from
 * the preceding term in O(1):
 * C(N, k) = C(N, k - 1) * (N - k + 1) / k
 *
 * Time Complexity: O(N)
 * Space Complexity: O(1) auxiliary (excluding output array).
 */

#include <iostream>
#include <vector>
#include <iomanip>

std::vector<long long> getPascalsRow(int rowIndex) {
    if (rowIndex < 0) return {};

    std::vector<long long> row(rowIndex + 1, 1);
    long long current = 1;

    for (int k = 1; k <= rowIndex; ++k) {
        current = current * (rowIndex - k + 1) / k;
        row[k] = current;
    }

    return row;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Pascal's Triangle N-th Row (O(N))     \n";
    std::cout << "==========================================\n\n";

    std::vector<int> testRows = {0, 1, 2, 4, 5, 10};

    for (int r : testRows) {
        auto row = getPascalsRow(r);
        std::cout << "Row " << std::setw(2) << r << ": [";
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << row[i] << (i + 1 < row.size() ? ", " : "");
        }
        std::cout << "]\n";

        // Verification: Sum of elements in row N must equal 2^N
        long long sum = 0;
        for (long long x : row) sum += x;
        long long expectedSum = 1LL << r;
        std::cout << "  Sum = " << sum << " (2^" << r << " = " << expectedSum << ") "
                  << (sum == expectedSum ? "[PASS]" : "[FAIL]") << "\n\n";
    }

    return 0;
}
