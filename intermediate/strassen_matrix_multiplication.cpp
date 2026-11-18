/**
 * ============================================================================
 * Strassen's Fast Matrix Multiplication Algorithm (Sub-Cubic O(N^2.807))
 * ============================================================================
 * 
 * Description:
 * Strassen's algorithm (discovered by Volker Strassen in 1969) is the first
 * sub-cubic matrix multiplication algorithm, proving that standard O(N^3)
 * matrix multiplication is not optimal.
 * 
 * Mathematical Formulation:
 * Consider multiplying two 2x2 block matrices A and B:
 *   [ A11  A12 ] * [ B11  B12 ] = [ C11  C12 ]
 *   [ A21  A22 ]   [ B21  B22 ]   [ C21  C22 ]
 * 
 * Standard block multiplication uses 8 block multiplications:
 *   T(N) = 8 * T(N/2) + O(N^2) => O(N^3).
 * 
 * Strassen reduced the required multiplications from 8 to 7:
 *   P1 = (A11 + A22) * (B11 + B22)
 *   P2 = (A21 + A22) * B11
 *   P3 = A11 * (B12 - B22)
 *   P4 = A22 * (B21 - B11)
 *   P5 = (A11 + A12) * B22
 *   P6 = (A21 - A11) * (B11 + B12)
 *   P7 = (A12 - A22) * (B21 + B22)
 * 
 * Recombining the results:
 *   C11 = P1 + P4 - P5 + P7
 *   C12 = P3 + P5
 *   C21 = P2 + P4
 *   C22 = P1 - P2 + P3 + P6
 * 
 * Recurrence & Complexity:
 *   T(N) = 7 * T(N/2) + O(N^2) => O(N^{log_2 7}) ~ O(N^{2.8074})
 * 
 * Time Complexity:
 * - O(N^{2.807}) vs standard O(N^3)
 * - Space Complexity: O(N^2) auxiliary for recursion blocks
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace strassen {

using Matrix = std::vector<std::vector<int64_t>>;

Matrix createMatrix(int n) {
    return Matrix(n, std::vector<int64_t>(n, 0));
}

Matrix add(const Matrix& A, const Matrix& B) {
    int n = static_cast<int>(A.size());
    Matrix C = createMatrix(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
    return C;
}

Matrix subtract(const Matrix& A, const Matrix& B) {
    int n = static_cast<int>(A.size());
    Matrix C = createMatrix(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
    return C;
}

Matrix standardMultiply(const Matrix& A, const Matrix& B) {
    int n = static_cast<int>(A.size());
    Matrix C = createMatrix(n);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            for (int j = 0; j < n; ++j) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

/**
 * Recursive Strassen matrix multiplication on power-of-two square matrices.
 */
Matrix strassenMultiplyRec(const Matrix& A, const Matrix& B) {
    int n = static_cast<int>(A.size());

    // Base case: for small dimensions, standard multiplication is faster
    if (n <= 2) {
        return standardMultiply(A, B);
    }

    int k = n / 2;
    Matrix A11 = createMatrix(k), A12 = createMatrix(k), A21 = createMatrix(k), A22 = createMatrix(k);
    Matrix B11 = createMatrix(k), B12 = createMatrix(k), B21 = createMatrix(k), B22 = createMatrix(k);

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + k];
            A21[i][j] = A[i + k][j];
            A22[i][j] = A[i + k][j + k];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][j + k];
            B21[i][j] = B[i + k][j];
            B22[i][j] = B[i + k][j + k];
        }
    }

    // 7 Strassen multiplications
    Matrix P1 = strassenMultiplyRec(add(A11, A22), add(B11, B22));
    Matrix P2 = strassenMultiplyRec(add(A21, A22), B11);
    Matrix P3 = strassenMultiplyRec(A11, subtract(B12, B22));
    Matrix P4 = strassenMultiplyRec(A22, subtract(B21, B11));
    Matrix P5 = strassenMultiplyRec(add(A11, A12), B22);
    Matrix P6 = strassenMultiplyRec(subtract(A21, A11), add(B11, B12));
    Matrix P7 = strassenMultiplyRec(subtract(A12, A22), add(B21, B22));

    // Combine into C
    Matrix C11 = add(subtract(add(P1, P4), P5), P7);
    Matrix C12 = add(P3, P5);
    Matrix C21 = add(P2, P4);
    Matrix C22 = add(subtract(add(P1, P3), P2), P6);

    Matrix C = createMatrix(n);
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < k; ++j) {
            C[i][j] = C11[i][j];
            C[i][j + k] = C12[i][j];
            C[i + k][j] = C21[i][j];
            C[i + k][j + k] = C22[i][j];
        }
    }

    return C;
}

/**
 * Top-level wrapper that pads matrices to next power of 2.
 */
Matrix multiply(const Matrix& A, const Matrix& B) {
    int orig_n = static_cast<int>(A.size());
    int m = 1;
    while (m < orig_n) m <<= 1;

    Matrix A_pad = createMatrix(m);
    Matrix B_pad = createMatrix(m);

    for (int i = 0; i < orig_n; ++i) {
        for (int j = 0; j < orig_n; ++j) {
            A_pad[i][j] = A[i][j];
            B_pad[i][j] = B[i][j];
        }
    }

    Matrix C_pad = strassenMultiplyRec(A_pad, B_pad);

    Matrix C = createMatrix(orig_n);
    for (int i = 0; i < orig_n; ++i) {
        for (int j = 0; j < orig_n; ++j) {
            C[i][j] = C_pad[i][j];
        }
    }
    return C;
}

} // namespace strassen

int main() {
    std::cout << "========================================================\n";
    std::cout << " Strassen's Fast Matrix Multiplication (O(N^2.807))\n";
    std::cout << "========================================================\n\n";

    // 4x4 test matrices
    strassen::Matrix A = {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {9, 1, 2, 3},
        {4, 5, 6, 7}
    };

    strassen::Matrix B = {
        {7, 6, 5, 4},
        {3, 2, 1, 0},
        {1, 2, 3, 4},
        {5, 6, 7, 8}
    };

    std::cout << "Matrix A (4x4):\n";
    for (const auto& row : A) {
        std::cout << "  ";
        for (int64_t v : row) std::cout << std::setw(5) << v;
        std::cout << "\n";
    }

    std::cout << "\nMatrix B (4x4):\n";
    for (const auto& row : B) {
        std::cout << "  ";
        for (int64_t v : row) std::cout << std::setw(5) << v;
        std::cout << "\n";
    }

    auto C_strassen = strassen::multiply(A, B);
    auto C_standard = strassen::standardMultiply(A, B);

    std::cout << "\nResult Matrix C via Strassen's Algorithm:\n";
    for (const auto& row : C_strassen) {
        std::cout << "  ";
        for (int64_t v : row) std::cout << std::setw(6) << v;
        std::cout << "\n";
    }

    bool matched = true;
    for (size_t i = 0; i < A.size(); ++i) {
        for (size_t j = 0; j < A.size(); ++j) {
            if (C_strassen[i][j] != C_standard[i][j]) {
                matched = false;
            }
        }
    }

    std::cout << "\nVerification against Standard O(N^3) Multiplication: "
              << (matched ? "PASSED (100% IDENTICAL)" : "FAILED") << "\n";

    return 0;
}
