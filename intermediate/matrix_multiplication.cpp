/*
 * Problem Description:
 * Write a program to multiply two matrices. The program should ask the user for the dimensions
 * of the two matrices, validate if they can be multiplied, take their elements as input, and
 * then print the resulting matrix after multiplication.
 */

#include <iostream>
#include <vector>

int main() {
    int r1, c1, r2, c2;
    std::cout << "Enter rows and columns for first matrix: ";
    std::cin >> r1 >> c1;
    std::cout << "Enter rows and columns for second matrix: ";
    std::cin >> r2 >> c2;

    if (c1 != r2) {
        std::cout << "Matrices cannot be multiplied!\n";
        return 1;
    }

    std::vector<std::vector<int>> a(r1, std::vector<int>(c1));
    std::vector<std::vector<int>> b(r2, std::vector<int>(c2));
    std::vector<std::vector<int>> mult(r1, std::vector<int>(c2, 0));

    std::cout << "Enter elements of matrix 1:\n";
    for(int i = 0; i < r1; ++i)
        for(int j = 0; j < c1; ++j)
            std::cin >> a[i][j];

    std::cout << "Enter elements of matrix 2:\n";
    for(int i = 0; i < r2; ++i)
        for(int j = 0; j < c2; ++j)
            std::cin >> b[i][j];

    for(int i = 0; i < r1; ++i)
        for(int j = 0; j < c2; ++j)
            for(int k = 0; k < c1; ++k)
                mult[i][j] += a[i][k] * b[k][j];

    std::cout << "Output Matrix:\n";
    for(int i = 0; i < r1; ++i) {
        for(int j = 0; j < c2; ++j)
            std::cout << mult[i][j] << " ";
        std::cout << "\n";
    }

    return 0;
}
