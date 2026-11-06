/**
 * @file matrix_qr_decomposition_gram_schmidt.cpp
 * @brief Computes QR Decomposition (A = Q * R) via Modified Gram-Schmidt and solves Linear Least Squares.
 *
 * Theory:
 * For an M x N matrix A with linearly independent columns (M >= N):
 *   A = Q * R
 * where:
 *   Q is an M x N matrix with orthonormal columns: Q^T * Q = I_N
 *   R is an N x N upper triangular matrix with positive diagonal entries.
 *
 * Modified Gram-Schmidt (MGS) Process:
 * MGS projects each column against the newly formed orthonormal vectors immediately,
 * avoiding the catastrophic cancellation and loss of orthogonality that plagues classical Gram-Schmidt.
 *
 * Application to Linear Least Squares:
 * To minimize ||A * x - b||^2:
 *   A * x = b  =>  Q * R * x = b  =>  R * x = Q^T * b
 * Since R is upper triangular, x is solved in O(N^2) via back-substitution.
 *
 * Time Complexity: O(M * N^2)
 * Space Complexity: O(M * N)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct QRResult {
    bool success;
    std::vector<std::vector<double>> Q;
    std::vector<std::vector<double>> R;
};

class ModifiedGramSchmidtQR {
public:
    static constexpr double EPS = 1e-11;

    static QRResult decompose(const std::vector<std::vector<double>>& A) {
        int m = static_cast<int>(A.size());
        int n = static_cast<int>(A[0].size());

        std::vector<std::vector<double>> Q(m, std::vector<double>(n, 0.0));
        std::vector<std::vector<double>> R(n, std::vector<double>(n, 0.0));

        // Initialize V as columns of A
        std::vector<std::vector<double>> V(n, std::vector<double>(m));
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                V[j][i] = A[i][j];
            }
        }

        for (int k = 0; k < n; ++k) {
            // Compute norm of V[k]
            double norm = 0.0;
            for (int i = 0; i < m; ++i) {
                norm += V[k][i] * V[k][i];
            }
            norm = std::sqrt(norm);

            if (norm < EPS) {
                return {false, {}, {}}; // Columns are linearly dependent
            }

            R[k][k] = norm;

            // Form orthonormal column Q[k]
            for (int i = 0; i < m; ++i) {
                Q[i][k] = V[k][i] / norm;
            }

            // Project remaining vectors against Q[k] (Modified Gram-Schmidt step)
            for (int j = k + 1; j < n; ++j) {
                double dot = 0.0;
                for (int i = 0; i < m; ++i) {
                    dot += Q[i][k] * V[j][i];
                }
                R[k][j] = dot;

                for (int i = 0; i < m; ++i) {
                    V[j][i] -= dot * Q[i][k];
                }
            }
        }

        return {true, Q, R};
    }

    /**
     * @brief Solves least squares problem min ||A * x - b|| via R * x = Q^T * b.
     */
    static std::vector<double> solveLeastSquares(
        const std::vector<std::vector<double>>& Q,
        const std::vector<std::vector<double>>& R,
        const std::vector<double>& b) {
        int m = static_cast<int>(Q.size());
        int n = static_cast<int>(R.size());

        // Compute d = Q^T * b
        std::vector<double> d(n, 0.0);
        for (int j = 0; j < n; ++j) {
            for (int i = 0; i < m; ++i) {
                d[j] += Q[i][j] * b[i];
            }
        }

        // Back substitution: R * x = d
        std::vector<double> x(n, 0.0);
        for (int i = n - 1; i >= 0; --i) {
            double sum = 0.0;
            for (int j = i + 1; j < n; ++j) {
                sum += R[i][j] * x[j];
            }
            x[i] = (d[i] - sum) / R[i][i];
        }

        return x;
    }
};

void printMatrix(const std::vector<std::vector<double>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (double val : row) {
            if (std::abs(val) < 1e-12) val = 0.0;
            std::cout << std::fixed << std::setprecision(4) << std::setw(9) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== Modified Gram-Schmidt QR Decomposition & Least Squares ===\n\n";

    // Overdetermined System A * x = b (3 equations, 2 unknowns)
    // 1x + 1y = 2
    // 1x + 2y = 3
    // 1x + 3y = 5
    std::vector<std::vector<double>> A = {
        {1.0, 1.0},
        {1.0, 2.0},
        {1.0, 3.0}
    };
    std::vector<double> b = {2.0, 3.0, 5.0};

    std::cout << "Design Matrix A (3x2):\n";
    printMatrix(A);

    auto qr = ModifiedGramSchmidtQR::decompose(A);
    if (qr.success) {
        std::cout << "\nOrthonormal Matrix Q (3x2):\n";
        printMatrix(qr.Q);

        std::cout << "\nUpper Triangular Matrix R (2x2):\n";
        printMatrix(qr.R);

        // Verify Q^T * Q == I
        std::cout << "\nVerification Q^T * Q:\n";
        int n = static_cast<int>(A[0].size());
        int m = static_cast<int>(A.size());
        std::vector<std::vector<double>> QtQ(n, std::vector<double>(n, 0.0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                for (int k = 0; k < m; ++k) {
                    QtQ[i][j] += qr.Q[k][i] * qr.Q[k][j];
                }
            }
        }
        printMatrix(QtQ);

        auto x = ModifiedGramSchmidtQR::solveLeastSquares(qr.Q, qr.R, b);
        std::cout << "\nOptimal Least-Squares Solution x*:\n  [ ";
        for (double val : x) {
            std::cout << std::fixed << std::setprecision(4) << val << " ";
        }
        std::cout << "]\n";
    }

    return 0;
}

