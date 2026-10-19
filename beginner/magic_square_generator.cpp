/*
 * Program Description:
 * Magic Square Generator for Odd Order (N x N) in C++.
 * A magic square is an N x N grid of numbers from 1 to N^2 such that the sum
 * of numbers in every row, column, and both diagonals is equal to the magic constant:
 * M = N * (N^2 + 1) / 2
 *
 * Algorithm (Siamese Method / De la Loubère Method):
 * 1. Start in the middle cell of the first row: (0, N / 2) with value 1.
 * 2. Next number is placed at (row - 1, col + 1) with wrap-around.
 * 3. If that cell is already occupied, place next number directly below: (row + 1, col).
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(N^2)
 */

#include <iostream>
#include <vector>
#include <iomanip>

// Generates an odd-order magic square
std::vector<std::vector<int>> generateOddMagicSquare(int n) {
    if (n % 2 == 0 || n <= 0) {
        return {}; // Method only valid for positive odd N
    }

    std::vector<std::vector<int>> square(n, std::vector<int>(n, 0));

    int row = 0;
    int col = n / 2;

    for (int num = 1; num <= n * n; ++num) {
        square[row][col] = num;

        int nextRow = (row - 1 + n) % n;
        int nextCol = (col + 1) % n;

        if (square[nextRow][nextCol] != 0) {
            // Cell already filled, drop down one row
            row = (row + 1) % n;
        } else {
            row = nextRow;
            col = nextCol;
        }
    }

    return square;
}

// Verifies magic square properties
bool verifyMagicSquare(const std::vector<std::vector<int>>& square, int n, int magicConstant) {
    // Check rows and columns
    for (int i = 0; i < n; ++i) {
        int rowSum = 0;
        int colSum = 0;
        for (int j = 0; j < n; ++j) {
            rowSum += square[i][j];
            colSum += square[j][i];
        }
        if (rowSum != magicConstant || colSum != magicConstant) return false;
    }

    // Check main and anti-diagonals
    int diag1 = 0, diag2 = 0;
    for (int i = 0; i < n; ++i) {
        diag1 += square[i][i];
        diag2 += square[i][n - 1 - i];
    }

    return (diag1 == magicConstant && diag2 == magicConstant);
}

void printMagicSquare(const std::vector<std::vector<int>>& square, int n) {
    int fieldWidth = 5;
    for (int i = 0; i < n; ++i) {
        std::cout << "  ";
        for (int j = 0; j < n; ++j) {
            std::cout << std::setw(fieldWidth) << square[i][j];
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Odd-Order Magic Square Generator       \n";
    std::cout << "==========================================\n\n";

    std::vector<int> orders = {3, 5};

    for (int n : orders) {
        int magicConstant = n * (n * n + 1) / 2;
        std::cout << "Magic Square of Order N = " << n << " (Sum = " << magicConstant << "):\n";

        auto square = generateOddMagicSquare(n);
        printMagicSquare(square, n);

        bool isValid = verifyMagicSquare(square, n, magicConstant);
        std::cout << "Verification Check: " << (isValid ? "ALL SUMS VALID [PASS]" : "FAILED") << "\n\n";
    }

    return 0;
}
