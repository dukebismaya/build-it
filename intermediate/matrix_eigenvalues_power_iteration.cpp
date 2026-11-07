/**
 * @file matrix_eigenvalues_power_iteration.cpp
 * @brief Dominant Eigenvalue and Eigenvector computation via Power Iteration in C++17.
 *
 * Power Iteration (also known as the Von Mises iteration) is an eigenvalue algorithm
 * that computes the greatest eigenvalue (in absolute magnitude) and its associated
 * eigenvector for a diagonalizable matrix A.
 *
 * Mathematical Formulation:
 *   Given an n x n matrix A with dominant eigenvalue lambda_1:
 *     |lambda_1| > |lambda_2| >= ... >= |lambda_n|
 *   Starting from a non-zero initial guess vector b_0:
 *     b_{k+1} = (A * b_k) / ||A * b_k||_2
 *   The sequence b_k converges to the normalized eigenvector corresponding to lambda_1.
 *   The Rayleigh quotient approximates the eigenvalue:
 *     mu_k = (b_k^T * A * b_k) / (b_k^T * b_k)
 *
 * Complexity:
 *   - Each iteration requires a matrix-vector multiplication: O(n^2).
 *   - If K iterations are needed for convergence: O(K * n^2) time, O(n) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>

namespace numerical {

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

/**
 * @brief Computes Euclidean norm (L2 norm) of a vector.
 */
double normL2(const Vector& v) {
    double sum = 0.0;
    for (double x : v) {
        sum += x * x;
    }
    return std::sqrt(sum);
}

/**
 * @brief Multiplies an n x n matrix with an n-dimensional vector.
 */
Vector matVecMul(const Matrix& A, const Vector& x) {
    size_t n = A.size();
    Vector result(n, 0.0);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[i] += A[i][j] * x[j];
        }
    }
    return result;
}

/**
 * @brief Computes dot product of two vectors.
 */
double dotProduct(const Vector& a, const Vector& b) {
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

struct EigenResult {
    double eigenvalue;
    Vector eigenvector;
    int iterations;
    bool converged;
};

/**
 * @brief Runs Power Iteration with Rayleigh Quotient approximation.
 * @param A Square matrix.
 * @param maxIter Maximum allowed iterations.
 * @param tol Convergence tolerance on consecutive eigenvalue differences.
 * @return EigenResult containing dominant eigenvalue and eigenvector.
 */
EigenResult powerIteration(const Matrix& A, int maxIter = 1000, double tol = 1e-9) {
    size_t n = A.size();
    Vector v(n, 1.0); // Initial guess vector
    double norm = normL2(v);
    for (double& val : v) val /= norm;

    double lambdaOld = 0.0;
    int iter = 0;

    for (; iter < maxIter; ++iter) {
        Vector w = matVecMul(A, v);
        double wNorm = normL2(w);
        if (wNorm < 1e-15) {
            return {0.0, v, iter, true};
        }

        // Normalize
        for (size_t i = 0; i < n; ++i) {
            v[i] = w[i] / wNorm;
        }

        // Rayleigh quotient: lambda = (v^T * A * v) / (v^T * v) = v^T * (A * v)
        Vector Av = matVecMul(A, v);
        double lambdaNew = dotProduct(v, Av);

        if (std::abs(lambdaNew - lambdaOld) < tol) {
            return {lambdaNew, v, iter + 1, true};
        }
        lambdaOld = lambdaNew;
    }

    return {lambdaOld, v, iter, false};
}

} // namespace numerical

int main() {
    std::cout << "========================================\n";
    std::cout << "   MATRIX EIGENVALUES (POWER ITERATION) \n";
    std::cout << "========================================\n\n";

    // Example 1: 3x3 Symmetric Matrix
    // A = [ [2, -1, 0],
    //       [-1, 2, -1],
    //       [0, -1, 2] ]
    // Analytical eigenvalues: 2 - sqrt(2) ~= 0.5857, 2.0, 2 + sqrt(2) ~= 3.4142
    numerical::Matrix A = {
        {2.0, -1.0,  0.0},
        {-1.0, 2.0, -1.0},
        {0.0, -1.0,  2.0}
    };

    std::cout << "Matrix A (3x3):\n";
    for (const auto& row : A) {
        std::cout << "  [ ";
        for (double val : row) {
            std::cout << std::setw(6) << std::fixed << std::setprecision(1) << val << " ";
        }
        std::cout << "]\n";
    }

    auto result = numerical::powerIteration(A, 500, 1e-10);

    std::cout << "\nPower Iteration Results:\n";
    std::cout << "  - Dominant Eigenvalue : " << std::setprecision(6) << result.eigenvalue
              << " (Theoretical: " << (2.0 + std::sqrt(2.0)) << ")\n";
    std::cout << "  - Iterations to converge: " << result.iterations << "\n";
    std::cout << "  - Converged?            : " << (result.converged ? "YES" : "NO") << "\n";
    std::cout << "  - Dominant Eigenvector  : [ ";
    for (size_t i = 0; i < result.eigenvector.size(); ++i) {
        std::cout << std::setprecision(6) << result.eigenvector[i]
                  << (i + 1 == result.eigenvector.size() ? "" : ", ");
    }
    std::cout << " ]\n\n";

    // Residual verification: ||A*v - lambda*v||
    auto Av = numerical::matVecMul(A, result.eigenvector);
    numerical::Vector residual(result.eigenvector.size());
    for (size_t i = 0; i < residual.size(); ++i) {
        residual[i] = Av[i] - result.eigenvalue * result.eigenvector[i];
    }
    double resNorm = numerical::normL2(residual);
    std::cout << "Residual Check ||A*v - lambda*v||: " << std::scientific << resNorm << "\n";

    std::cout << "\nProgram finished successfully.\n";
    return 0;
}
