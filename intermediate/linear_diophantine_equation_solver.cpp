/**
 * @file linear_diophantine_equation_solver.cpp
 * @brief Complete solver and integer parametrization for Linear Diophantine Equations in C++17.
 *
 * A Linear Diophantine Equation in two variables has the form:
 *     a * x + b * y = c
 * where a, b, c are given integers and x, y are integer unknowns.
 *
 * Existence of Solutions:
 *   By Bezout's Identity, an integer solution exists if and only if:
 *     gcd(a, b) divides c.
 *
 * General Solution Parametrization:
 *   Let g = gcd(a, b). Using the Extended Euclidean Algorithm:
 *     a * x_0 + b * y_0 = g
 *   A particular solution to a*x + b*y = c is:
 *     x_p = x_0 * (c / g)
 *     y_p = y_0 * (c / g)
 *   All integer solutions (x, y) are parametrized for all integers k in Z:
 *     x(k) = x_p + k * (b / g)
 *     y(k) = y_p - k * (a / g)
 *
 * Complexity:
 *   - Extended Euclidean Algorithm: O(log(min(|a|, |b|))) time, O(1) space.
 *   - Solution evaluation: O(1) per k.
 */

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <cmath>
#include <optional>
#include <vector>

namespace diophantine {

/**
 * @brief Extended Euclidean Algorithm.
 * Computes integers x and y such that a*x + b*y = gcd(a, b).
 * @return std::pair<int64_t, int64_t> (x, y), and sets gcd out-param.
 */
int64_t extGCD(int64_t a, int64_t b, int64_t& x, int64_t& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    int64_t x1, y1;
    int64_t d = extGCD(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
}

struct SolutionParametrization {
    int64_t x_p;   // Particular x
    int64_t y_p;   // Particular y
    int64_t dx;    // Step size for x: b / gcd(a, b)
    int64_t dy;    // Step size for y: -a / gcd(a, b)
    int64_t gcdVal;

    std::pair<int64_t, int64_t> getAt(int64_t k) const {
        return {x_p + k * dx, y_p + k * dy};
    }
};

/**
 * @brief Solves a * x + b * y = c.
 * @return std::optional with SolutionParametrization if solvable, std::nullopt otherwise.
 */
std::optional<SolutionParametrization> solve(int64_t a, int64_t b, int64_t c) {
    if (a == 0 && b == 0) {
        if (c == 0) {
            return SolutionParametrization{0, 0, 0, 0, 0};
        }
        return std::nullopt;
    }

    int64_t x0, y0;
    int64_t g = extGCD(std::abs(a), std::abs(b), x0, y0);

    if (c % g != 0) {
        return std::nullopt; // No integer solution
    }

    if (a < 0) x0 = -x0;
    if (b < 0) y0 = -y0;

    int64_t factor = c / g;
    int64_t xp = x0 * factor;
    int64_t yp = y0 * factor;

    int64_t dx = b / g;
    int64_t dy = -a / g;

    return SolutionParametrization{xp, yp, dx, dy, g};
}

} // namespace diophantine

int main() {
    std::cout << "========================================\n";
    std::cout << "  LINEAR DIOPHANTINE EQUATION SOLVER    \n";
    std::cout << "========================================\n\n";

    struct TestCase {
        int64_t a, b, c;
    };

    std::vector<TestCase> cases = {
        {15, 25, 35},  // gcd(15, 25) = 5 | 35 -> Solvable
        {14, 30, 26},  // gcd(14, 30) = 2 | 26 -> Solvable
        {6, 9, 10},    // gcd(6, 9) = 3 does not divide 10 -> Unsolvable
        {105, 252, 21} // gcd(105, 252) = 21 | 21 -> Solvable
    };

    for (const auto& tc : cases) {
        std::cout << "Equation: " << tc.a << "*x + " << tc.b << "*y = " << tc.c << "\n";
        auto sol = diophantine::solve(tc.a, tc.b, tc.c);

        if (!sol) {
            std::cout << "  Result: NO INTEGER SOLUTION EXISTS.\n\n";
        } else {
            std::cout << "  gcd(" << tc.a << ", " << tc.b << ") = " << sol->gcdVal << "\n";
            std::cout << "  Particular Solution: (x_0, y_0) = (" << sol->x_p << ", " << sol->y_p << ")\n";
            std::cout << "  General Form: x(k) = " << sol->x_p << " + (" << sol->dx << ") * k\n";
            std::cout << "                y(k) = " << sol->y_p << " + (" << sol->dy << ") * k\n";
            std::cout << "  Sample solutions for k in [-2, 2]:\n";
            for (int64_t k = -2; k <= 2; ++k) {
                auto [xk, yk] = sol->getAt(k);
                int64_t check = tc.a * xk + tc.b * yk;
                std::cout << "    k = " << std::setw(2) << k
                          << " -> (x, y) = (" << std::setw(5) << xk << ", " << std::setw(5) << yk << ")"
                          << " | check: " << tc.a << "*(" << xk << ") + " << tc.b << "*(" << yk << ") = " << check << "\n";
            }
            std::cout << "\n";
        }
    }

    std::cout << "Program finished successfully.\n";
    return 0;
}
