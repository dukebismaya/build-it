/**
 * @file gaussian_elimination_modulo_p.cpp
 * @brief Solves systems of linear equations over a prime finite field F_p (Z / pZ) using Gaussian Elimination.
 *
 * Theory:
 * - A system of linear equations A * x = b (mod p), where p is prime.
 * - Arithmetic is carried out in the Galois field GF(p) = F_p.
 * - Division by a non-zero element 'a' mod p is multiplication by its modular inverse:
 *     inv(a, p) = a^(p - 2) mod p  (by Fermat's Little Theorem)
 *
 * Classification of Solutions:
 * 1. No solution: Inconsistent row [0 0 ... 0 | c] with c != 0 (mod p).
 * 2. Unique solution: Rank equals number of variables m (exactly one solution).
 * 3. Multiple solutions: Consistent system with rank < m (total solutions = p^(m - rank)).
 *
 * Time Complexity: O(min(N, M) * N * M) ~ O(N^3) for square systems
 * Space Complexity: O(N * M)
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

enum class SystemSolutionStatus {
    UNIQUE_SOLUTION,
    INFINITE_SOLUTIONS,
    NO_SOLUTION
};

struct ModularSystemResult {
    SystemSolutionStatus status;
    std::vector<long long> solution; // Particular solution (if any)
    int rank;
    long long totalSolutionsCount;   // p^(m - rank) if infinite, 1 if unique, 0 if none
};

class ModularGaussianElimination {
private:
    long long p; // Prime modulus

    long long mod(long long x) const {
        x %= p;
        if (x < 0) x += p;
        return x;
    }

    long long power(long long base, long long exp) const {
        long long res = 1;
        base = mod(base);
        while (exp > 0) {
            if (exp & 1) res = (res * base) % p;
            base = (base * base) % p;
            exp >>= 1;
        }
        return res;
    }

    long long modInverse(long long a) const {
        return power(a, p - 2); // Fermat's Little Theorem
    }

public:
    explicit ModularGaussianElimination(long long primeModulus) : p(primeModulus) {}

    /**
     * @brief Solves A * x = b (mod p).
     * @param A Coefficient matrix of size N x M.
     * @param b Right-hand side vector of size N.
     * @return ModularSystemResult with status, particular solution, rank, and count.
     */
    ModularSystemResult solve(std::vector<std::vector<long long>> A, std::vector<long long> b) const {
        int n = static_cast<int>(A.size());
        int m = static_cast<int>(A[0].size());

        // Create augmented matrix [A | b] of size n x (m + 1)
        std::vector<std::vector<long long>> aug(n, std::vector<long long>(m + 1, 0));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                aug[i][j] = mod(A[i][j]);
            }
            aug[i][m] = mod(b[i]);
        }

        std::vector<int> pivotCol(n, -1);
        int row = 0;

        for (int col = 0; col < m && row < n; ++col) {
            // Find pivot
            int sel = -1;
            for (int i = row; i < n; ++i) {
                if (aug[i][col] != 0) {
                    sel = i;
                    break;
                }
            }

            if (sel == -1) continue;

            std::swap(aug[row], aug[sel]);
            pivotCol[row] = col;

            // Normalize pivot row
            long long invPivot = modInverse(aug[row][col]);
            for (int j = col; j <= m; ++j) {
                aug[row][j] = (aug[row][j] * invPivot) % p;
            }

            // Eliminate column entries in other rows
            for (int i = 0; i < n; ++i) {
                if (i != row && aug[i][col] != 0) {
                    long long factor = aug[i][col];
                    for (int j = col; j <= m; ++j) {
                        aug[i][j] = mod(aug[i][j] - factor * aug[row][j]);
                    }
                }
            }

            row++;
        }

        int rank = row;

        // Check for inconsistency (0 == c with c != 0)
        for (int i = row; i < n; ++i) {
            if (aug[i][m] != 0) {
                return {SystemSolutionStatus::NO_SOLUTION, {}, rank, 0};
            }
        }

        // Construct particular solution
        std::vector<long long> ans(m, 0);
        for (int i = 0; i < row; ++i) {
            ans[pivotCol[i]] = aug[i][m];
        }

        if (rank < m) {
            // Infinite solutions in F_p: p^(m - rank)
            long long solCount = power(p, m - rank);
            return {SystemSolutionStatus::INFINITE_SOLUTIONS, ans, rank, solCount};
        }

        return {SystemSolutionStatus::UNIQUE_SOLUTION, ans, rank, 1};
    }
};

void printSystem(const std::vector<std::vector<long long>>& A, const std::vector<long long>& b, long long p) {
    int n = static_cast<int>(A.size());
    int m = static_cast<int>(A[0].size());
    for (int i = 0; i < n; ++i) {
        std::cout << "  ";
        for (int j = 0; j < m; ++j) {
            std::cout << std::setw(3) << A[i][j] << " x" << j;
            if (j + 1 < m) std::cout << " + ";
        }
        std::cout << " = " << std::setw(2) << b[i] << " (mod " << p << ")\n";
    }
}

int main() {
    std::cout << "=== Gaussian Elimination over Finite Prime Field F_p ===\n\n";

    // Example 1: 3x3 System mod 7 with unique solution
    // 2x0 + 1x1 + 3x2 = 5 (mod 7)
    // 1x0 + 4x1 + 2x2 = 3 (mod 7)
    // 3x0 + 2x1 + 1x2 = 6 (mod 7)
    long long p1 = 7;
    std::vector<std::vector<long long>> A1 = {
        {2, 1, 3},
        {1, 4, 2},
        {3, 2, 1}
    };
    std::vector<long long> b1 = {5, 3, 6};

    std::cout << "1. System 1 (Modulus p = " << p1 << "):\n";
    printSystem(A1, b1, p1);

    ModularGaussianElimination solver1(p1);
    auto res1 = solver1.solve(A1, b1);

    if (res1.status == SystemSolutionStatus::UNIQUE_SOLUTION) {
        std::cout << "Status: UNIQUE SOLUTION FOUND (Rank = " << res1.rank << ")\n";
        std::cout << "Solution: [ ";
        for (size_t i = 0; i < res1.solution.size(); ++i) {
            std::cout << "x" << i << " = " << res1.solution[i] << " ";
        }
        std::cout << "]\n\n";
    }

    // Example 2: Inconsistent system (No solution) mod 5
    // 1x0 + 2x1 = 3 (mod 5)
    // 2x0 + 4x1 = 2 (mod 5)  [since 2*(1x0 + 2x1) = 2*3 = 6 = 1 != 2 (mod 5)]
    long long p2 = 5;
    std::vector<std::vector<long long>> A2 = {
        {1, 2},
        {2, 4}
    };
    std::vector<long long> b2 = {3, 2};

    std::cout << "2. System 2 (Modulus p = " << p2 << "):\n";
    printSystem(A2, b2, p2);

    ModularGaussianElimination solver2(p2);
    auto res2 = solver2.solve(A2, b2);
    std::cout << "Status: " 
              << (res2.status == SystemSolutionStatus::NO_SOLUTION ? "NO SOLUTION (Inconsistent)" : "SOLVABLE")
              << " (Rank = " << res2.rank << ")\n\n";

    // Example 3: Underdetermined system (Infinite / Multiple solutions) mod 11
    // 1x0 + 3x1 + 2x2 = 4 (mod 11)
    // 2x0 + 6x1 + 4x2 = 8 (mod 11)
    long long p3 = 11;
    std::vector<std::vector<long long>> A3 = {
        {1, 3, 2},
        {2, 6, 4}
    };
    std::vector<long long> b3 = {4, 8};

    std::cout << "3. System 3 (Modulus p = " << p3 << "):\n";
    printSystem(A3, b3, p3);

    ModularGaussianElimination solver3(p3);
    auto res3 = solver3.solve(A3, b3);

    if (res3.status == SystemSolutionStatus::INFINITE_SOLUTIONS) {
        std::cout << "Status: MULTIPLE SOLUTIONS IN F_" << p3 << " (Rank = " << res3.rank << ")\n";
        std::cout << "Particular Solution: [ ";
        for (size_t i = 0; i < res3.solution.size(); ++i) {
            std::cout << "x" << i << " = " << res3.solution[i] << " ";
        }
        std::cout << "]\n";
        std::cout << "Total Valid Vector Solutions (p^(m-rank)): " << res3.totalSolutionsCount << '\n';
    }

    return 0;
}
