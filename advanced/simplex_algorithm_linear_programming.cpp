/**
 * @file simplex_algorithm_linear_programming.cpp
 * @brief Implements the Simplex Algorithm for Linear Programming (LP) optimization.
 *
 * Theory:
 * Invented by George Dantzig in 1947.
 * Solves standard form Linear Programming problems:
 *   Maximize:    Z = c^T * x
 *   Subject to:  A * x <= b
 *                x >= 0
 *
 * Simplex Tableau Method:
 * 1. Add slack variables s_1, s_2, ..., s_m to convert inequalities into equalities:
 *      A * x + I * s = b
 * 2. Pivot Selection:
 *    - Entering variable: Column with most negative coefficient in the objective row.
 *    - Leaving variable: Row that minimizes the ratio test (b_i / A_i,j) for A_i,j > 0.
 * 3. Pivot Operation:
 *    - Normalize pivot row.
 *    - Eliminate column entry from all other rows.
 * 4. Termination:
 *    - Optimal: All coefficients in objective row >= 0.
 *    - Unbounded: All entries in pivot column <= 0.
 *
 * Time Complexity: Exponential worst-case, but polynomial O(m^2 * n) average-case in practice
 * Space Complexity: O(m * n) for the tableau
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

enum class LPStatus {
    OPTIMAL,
    UNBOUNDED,
    INFEASIBLE
};

struct LPSolution {
    LPStatus status;
    double optimalValue;
    std::vector<double> variables;
};

class SimplexSolver {
private:
    static constexpr double EPS = 1e-9;

    int m; // Number of constraints
    int n; // Number of original variables
    std::vector<std::vector<double>> tableau;
    std::vector<int> basicVars;

public:
    /**
     * @brief Initializes standard maximization LP: Maximize c^T * x subject to A * x <= b, x >= 0.
     * @param A Constraint matrix (m x n).
     * @param b Right-hand side bounds (size m, must have b_i >= 0).
     * @param c Objective coefficients (size n).
     */
    SimplexSolver(const std::vector<std::vector<double>>& A,
                  const std::vector<double>& b,
                  const std::vector<double>& c) {
        m = static_cast<int>(A.size());
        n = static_cast<int>(c.size());

        // Tableau dimensions: (m + 1) rows, (n + m + 1) columns
        tableau.assign(m + 1, std::vector<double>(n + m + 1, 0.0));
        basicVars.resize(m);

        // Fill constraints with slack variables
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                tableau[i][j] = A[i][j];
            }
            tableau[i][n + i] = 1.0;     // Slack variable
            tableau[i][n + m] = b[i];     // RHS
            basicVars[i] = n + i;        // Slack is initially basic
        }

        // Objective row: Z - sum(c_j * x_j) = 0 => row entries are -c_j
        for (int j = 0; j < n; ++j) {
            tableau[m][j] = -c[j];
        }
    }

    LPSolution solve() {
        while (true) {
            // 1. Find entering variable (most negative in objective row)
            int pivotCol = -1;
            double minVal = -EPS;

            for (int j = 0; j < n + m; ++j) {
                if (tableau[m][j] < minVal) {
                    minVal = tableau[m][j];
                    pivotCol = j;
                }
            }

            // If no negative entry exists, optimal solution is reached
            if (pivotCol == -1) {
                break;
            }

            // 2. Minimum ratio test to find leaving variable
            int pivotRow = -1;
            double minRatio = 1e18;

            for (int i = 0; i < m; ++i) {
                if (tableau[i][pivotCol] > EPS) {
                    double ratio = tableau[i][n + m] / tableau[i][pivotCol];
                    if (ratio < minRatio) {
                        minRatio = ratio;
                        pivotRow = i;
                    }
                }
            }

            if (pivotRow == -1) {
                return {LPStatus::UNBOUNDED, 0.0, {}};
            }

            // 3. Pivot on (pivotRow, pivotCol)
            double pivotElement = tableau[pivotRow][pivotCol];
            for (int j = 0; j <= n + m; ++j) {
                tableau[pivotRow][j] /= pivotElement;
            }

            for (int i = 0; i <= m; ++i) {
                if (i != pivotRow && std::abs(tableau[i][pivotCol]) > EPS) {
                    double factor = tableau[i][pivotCol];
                    for (int j = 0; j <= n + m; ++j) {
                        tableau[i][j] -= factor * tableau[pivotRow][j];
                    }
                }
            }

            basicVars[pivotRow] = pivotCol;
        }

        // Extract solution
        std::vector<double> x(n, 0.0);
        for (int i = 0; i < m; ++i) {
            if (basicVars[i] < n) {
                x[basicVars[i]] = tableau[i][n + m];
            }
        }

        double optimalVal = tableau[m][n + m];
        return {LPStatus::OPTIMAL, optimalVal, x};
    }
};

int main() {
    std::cout << "=== Simplex Algorithm for Linear Programming ===\n\n";

    // Standard Linear Program:
    // Maximize Z = 3*x1 + 2*x2
    // Subject to:
    //   2*x1 + 1*x2 <= 18
    //   2*x1 + 3*x2 <= 42
    //   3*x1 + 1*x2 <= 24
    //   x1, x2 >= 0
    std::vector<std::vector<double>> A = {
        {2.0, 1.0},
        {2.0, 3.0},
        {3.0, 1.0}
    };
    std::vector<double> b = {18.0, 42.0, 24.0};
    std::vector<double> c = {3.0, 2.0};

    std::cout << "Linear Program Specification:\n";
    std::cout << "  Maximize Z = 3*x1 + 2*x2\n";
    std::cout << "  Subject to:\n";
    std::cout << "    2*x1 + 1*x2 <= 18\n";
    std::cout << "    2*x1 + 3*x2 <= 42\n";
    std::cout << "    3*x1 + 1*x2 <= 24\n";
    std::cout << "    x1, x2 >= 0\n\n";

    SimplexSolver solver(A, b, c);
    auto solution = solver.solve();

    if (solution.status == LPStatus::OPTIMAL) {
        std::cout << "Optimal Solution Found:\n";
        std::cout << "  Maximum Objective Value Z* = " << std::fixed << std::setprecision(2) 
                  << solution.optimalValue << '\n';
        for (size_t i = 0; i < solution.variables.size(); ++i) {
            std::cout << "  x" << (i + 1) << "* = " << solution.variables[i] << '\n';
        }
    } else if (solution.status == LPStatus::UNBOUNDED) {
        std::cout << "Linear Program is Unbounded!\n";
    }

    return 0;
}

