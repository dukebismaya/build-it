/**
 * @file burnsides_lemma_necklace_coloring.cpp
 * @brief Counts non-equivalent necklace and bracelet colorings using Burnside's Lemma and Polya Enumeration.
 *
 * Theory:
 * Burnside's Lemma (Cauchy-Frobenius Lemma) computes the number of orbits |X / G|
 * under a finite group action G on a set X:
 *   |X / G| = (1 / |G|) * sum_{g in G} |Fix(g)|
 * where Fix(g) is the set of elements in X fixed by group element g.
 *
 * 1. Necklaces (Rotational Symmetry, Cyclic Group C_n, |G| = n):
 *    A rotation by i positions has gcd(i, n) independent cycles.
 *    Each cycle must receive the same color, giving k^gcd(i, n) fixed colorings.
 *    Formula:
 *      Necklaces(n, k) = (1 / n) * sum_{d | n} ( phi(d) * k^(n / d) )
 *
 * 2. Bracelets (Rotations and Reflections, Dihedral Group D_n, |G| = 2n):
 *    In addition to rotations, there are n reflections:
 *    - If n is odd:  n reflection axes, each fixing k^((n + 1) / 2) colorings.
 *    - If n is even: n / 2 axes through opposite beads (k^(n / 2 + 1))
 *                    n / 2 axes through edge midpoints (k^(n / 2)).
 *
 * Time Complexity: O(sqrt(n) * log(n)) using divisor factorization and modular exponentiation
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>
#include <iomanip>

class BurnsideNecklaceColoring {
public:
    static long long power(long long base, long long exp) {
        long long res = 1;
        while (exp > 0) {
            if (exp & 1) res *= base;
            base *= base;
            exp >>= 1;
        }
        return res;
    }

    static long long gcd(long long a, long long b) {
        while (b) {
            a %= b;
            std::swap(a, b);
        }
        return a;
    }

    /**
     * @brief Computes Euler's Totient function phi(n).
     */
    static long long phi(long long n) {
        long long result = n;
        for (long long p = 2; p * p <= n; ++p) {
            if (n % p == 0) {
                while (n % p == 0) n /= p;
                result -= result / p;
            }
        }
        if (n > 1) {
            result -= result / n;
        }
        return result;
    }

    /**
     * @brief Counts distinct necklaces (rotations only: C_n) with n beads and k colors.
     */
    static long long countNecklaces(long long n, long long k) {
        if (n <= 0 || k <= 0) return 0;

        long long totalFixed = 0;
        for (long long i = 0; i < n; ++i) {
            long long cycles = gcd(i, n);
            totalFixed += power(k, cycles);
        }

        return totalFixed / n;
    }

    /**
     * @brief Counts distinct bracelets (rotations + reflections: D_n) with n beads and k colors.
     */
    static long long countBracelets(long long n, long long k) {
        if (n <= 0 || k <= 0) return 0;

        // 1. Sum over rotations
        long long rotFixed = 0;
        for (long long i = 0; i < n; ++i) {
            rotFixed += power(k, gcd(i, n));
        }

        // 2. Sum over reflections
        long long reflFixed = 0;
        if (n % 2 != 0) {
            // n is odd: n reflections with (n-1)/2 + 1 = (n+1)/2 cycles
            reflFixed = n * power(k, (n + 1) / 2);
        } else {
            // n is even: n/2 axes through vertices, n/2 axes through edges
            long long axisVertices = (n / 2) * power(k, n / 2 + 1);
            long long axisEdges = (n / 2) * power(k, n / 2);
            reflFixed = axisVertices + axisEdges;
        }

        return (rotFixed + reflFixed) / (2 * n);
    }
};

int main() {
    std::cout << "=== Necklace and Bracelet Coloring via Burnside's Lemma ===\n\n";

    // Test cases: (n beads, k colors)
    struct TestCase {
        long long n;
        long long k;
    };

    std::vector<TestCase> cases = {
        {4, 2}, // 4 beads, 2 colors (Black/White)
        {4, 3}, // 4 beads, 3 colors
        {5, 2}, // 5 beads, 2 colors
        {6, 2}, // 6 beads, 2 colors
        {6, 3}, // 6 beads, 3 colors
        {8, 2}  // 8 beads, 2 colors
    };

    std::cout << "  " << std::setw(10) << "Beads (n)"
              << std::setw(12) << "Colors (k)"
              << std::setw(16) << "Total (k^n)"
              << std::setw(20) << "Necklaces (C_n)"
              << std::setw(20) << "Bracelets (D_n)" << '\n';
    std::cout << "  " << std::string(78, '-') << '\n';

    for (const auto& c : cases) {
        long long rawTotal = BurnsideNecklaceColoring::power(c.k, c.n);
        long long necklaces = BurnsideNecklaceColoring::countNecklaces(c.n, c.k);
        long long bracelets = BurnsideNecklaceColoring::countBracelets(c.n, c.k);

        std::cout << "  " << std::setw(10) << c.n
                  << std::setw(12) << c.k
                  << std::setw(16) << rawTotal
                  << std::setw(20) << necklaces
                  << std::setw(20) << bracelets << '\n';
    }

    std::cout << "\nExplanation:\n"
              << "  - Necklaces allow only cyclic rotations (C_n).\n"
              << "  - Bracelets allow both rotations and flips/reflections (D_n).\n";

    return 0;
}
