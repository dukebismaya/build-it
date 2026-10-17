/*
 * Program Description:
 * Extended Euclidean Algorithm and Applications.
 * Given integers a and b, computes:
 * 1. gcd(a, b)
 * 2. Bézout coefficients (x, y) such that: a * x + b * y = gcd(a, b)
 * 3. Modular Multiplicative Inverse: a * x ≡ 1 (mod m)
 * 4. General solution to Linear Diophantine Equation: a * x + b * y = c
 *
 * Time Complexity: O(log(min(a, b)))
 * Space Complexity: O(log(min(a, b))) recursive or O(1) iterative
 */

#include <iostream>
#include <tuple>
#include <optional>

// Extended Euclidean Algorithm (returns tuple <gcd, x, y>)
std::tuple<long long, long long, long long> extendedGCD(long long a, long long b) {
    if (b == 0) {
        return {a, 1, 0};
    }

    auto [g, x1, y1] = extendedGCD(b, a % b);
    long long x = y1;
    long long y = x1 - (a / b) * y1;

    return {g, x, y};
}

// Computes modular multiplicative inverse of a modulo m
// Returns x such that (a * x) % m == 1
std::optional<long long> modInverse(long long a, long long m) {
    auto [g, x, y] = extendedGCD(a, m);
    if (g != 1) {
        return std::nullopt; // Inverse does not exist when a and m are not coprime
    }
    // Ensure result is positive
    long long inv = (x % m + m) % m;
    return inv;
}

// Solves linear Diophantine equation: a * x + b * y = c
// Returns a particular solution (x0, y0) if one exists
std::optional<std::pair<long long, long long>> solveDiophantine(long long a, long long b, long long c) {
    auto [g, x0, y0] = extendedGCD(std::abs(a), std::abs(b));

    if (c % g != 0) {
        return std::nullopt; // No integer solution exists
    }

    x0 *= (c / g);
    y0 *= (c / g);

    if (a < 0) x0 = -x0;
    if (b < 0) y0 = -y0;

    return std::make_pair(x0, y0);
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Extended Euclidean Algorithm        \n";
    std::cout << "==========================================\n\n";

    long long a = 240, b = 46;
    auto [g, x, y] = extendedGCD(a, b);

    std::cout << "Bézout's Identity for a = " << a << ", b = " << b << ":\n";
    std::cout << "  gcd(" << a << ", " << b << ") = " << g << "\n";
    std::cout << "  Coefficients: x = " << x << ", y = " << y << "\n";
    std::cout << "  Verification: " << a << "*(" << x << ") + " << b << "*(" << y << ") = "
              << (a * x + b * y) << " (Matches " << g << ")\n\n";

    std::cout << "--- Modular Multiplicative Inverse ---\n";
    long long num = 3, mod = 11;
    auto inv = modInverse(num, mod);
    if (inv) {
        std::cout << "  " << num << "^(-1) mod " << mod << " = " << *inv << "\n";
        std::cout << "  Check: (" << num << " * " << *inv << ") % " << mod << " = "
                  << (num * (*inv)) % mod << "\n";
    }

    std::cout << "\n--- Linear Diophantine Equation Solver ---\n";
    // Equation: 21x + 14y = 35
    long long da = 21, db = 14, dc = 35;
    std::cout << "Solving: " << da << "x + " << db << "y = " << dc << "\n";
    auto solution = solveDiophantine(da, db, dc);
    if (solution) {
        std::cout << "  Particular Solution: x0 = " << solution->first
                  << ", y0 = " << solution->second << "\n";
        std::cout << "  Verification: " << da << "*(" << solution->first << ") + "
                  << db << "*(" << solution->second << ") = "
                  << (da * solution->first + db * solution->second) << "\n";
    } else {
        std::cout << "  No integer solution exists.\n";
    }

    return 0;
}
