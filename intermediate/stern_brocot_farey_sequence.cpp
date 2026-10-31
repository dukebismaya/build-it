/**
 * @file stern_brocot_farey_sequence.cpp
 * @brief Generates Farey Sequences and finds best rational approximations via the Stern-Brocot Tree.
 *
 * Theory:
 * 1. Farey Sequence F_n:
 *    The sequence of completely reduced fractions between 0 and 1 with denominators <= n,
 *    arranged in strictly increasing order.
 *    Starting term: 0/1, Ending term: 1/1.
 *
 *    Farey Recurrence (generating successive terms in O(1) time without computing GCD):
 *    If a/b and c/d are two consecutive terms in F_n, the subsequent term e/f is:
 *      k = floor((n + b) / d)
 *      e = k * c - a
 *      f = k * d - b
 *
 * 2. Farey Property:
 *    For any two consecutive fractions a/b < c/d in F_n:
 *      b * c - a * d = 1  (Unimodular determinant)
 *
 * 3. Stern-Brocot Tree Rational Approximation:
 *    Binary search on the Stern-Brocot tree using the mediant (a + c) / (b + d) to find the
 *    closest fraction p/q to any target real number with denominator q <= maxDenominator.
 *
 * Time Complexity:
 * - Farey Sequence Generation: O(|F_n|) = O(n^2)
 * - Stern-Brocot Rational Approximation: O(log(maxDenominator))
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

struct Fraction {
    long long num;
    long long den;

    double toDouble() const {
        return static_cast<double>(num) / static_cast<double>(den);
    }
};

/**
 * @brief Generates the Farey sequence of order n using the iterative 3-term recurrence.
 * @param n Order of the Farey sequence (maximum denominator).
 * @return Vector of irreducible fractions in ascending order.
 */
std::vector<Fraction> generateFareySequence(int n) {
    if (n < 1) return {};

    std::vector<Fraction> farey;

    // First two terms: 0/1 and 1/n
    long long a = 0, b = 1;
    long long c = 1, d = n;

    farey.push_back({a, b});
    farey.push_back({c, d});

    while (c < n) {
        long long k = (n + b) / d;
        long long nextNum = k * c - a;
        long long nextDen = k * d - b;

        farey.push_back({nextNum, nextDen});

        a = c;
        b = d;
        c = nextNum;
        d = nextDen;
    }

    return farey;
}

/**
 * @brief Finds the best rational approximation to a target in (0, 1) using the Stern-Brocot tree.
 * @param target Real value between 0.0 and 1.0.
 * @param maxDen Maximum allowed denominator.
 * @return Best approximating Fraction.
 */
Fraction approximateReal(double target, long long maxDen) {
    long long a = 0, b = 1; // Left boundary: 0/1
    long long c = 1, d = 1; // Right boundary: 1/1

    Fraction best = {0, 1};
    double minDiff = std::abs(target - best.toDouble());

    while (true) {
        long long medNum = a + c;
        long long medDen = b + d;

        if (medDen > maxDen) break;

        double medVal = static_cast<double>(medNum) / static_cast<double>(medDen);
        double diff = std::abs(target - medVal);

        if (diff < minDiff) {
            minDiff = diff;
            best = {medNum, medDen};
        }

        if (std::abs(diff) < 1e-15) break;

        if (target > medVal) {
            a = medNum;
            b = medDen;
        } else {
            c = medNum;
            d = medDen;
        }
    }

    return best;
}

int main() {
    std::cout << "=== Farey Sequence and Stern-Brocot Rational Approximation ===\n\n";

    // 1. Generate Farey Sequence F_5
    int n = 5;
    auto f5 = generateFareySequence(n);

    std::cout << "1. Farey Sequence of Order F_" << n << " (" << f5.size() << " terms):\n  ";
    for (size_t i = 0; i < f5.size(); ++i) {
        std::cout << f5[i].num << "/" << f5[i].den;
        if (i + 1 < f5.size()) std::cout << " < ";
    }
    std::cout << "\n\n";

    // Verify Farey property: b * c - a * d == 1 for all adjacent pairs
    std::cout << "  Verifying unimodular property (b * c - a * d == 1):\n";
    bool allUnimodular = true;
    for (size_t i = 0; i + 1 < f5.size(); ++i) {
        long long cross = f5[i].den * f5[i + 1].num - f5[i].num * f5[i + 1].den;
        if (cross != 1) {
            allUnimodular = false;
        }
    }
    std::cout << "  Unimodular check across all consecutive pairs: "
              << (allUnimodular ? "PASS (All differences equal 1)" : "FAIL") << "\n\n";

    // 2. Stern-Brocot Tree Rational Approximations
    std::cout << "2. Stern-Brocot Best Rational Approximations:\n";

    // Approximate fractional part of pi: pi - 3 = 0.141592653589793...
    const double piFrac = 3.14159265358979323846 - 3.0;
    std::vector<long long> maxDenominators = {10, 100, 500, 10000, 1000000};

    std::cout << "  Approximating (pi - 3) = " << std::fixed << std::setprecision(10) << piFrac << ":\n";
    std::cout << "  " << std::setw(14) << "Max Denom"
              << std::setw(16) << "Fraction"
              << std::setw(20) << "Decimal Value"
              << std::setw(18) << "Absolute Error" << '\n';
    std::cout << "  " << std::string(68, '-') << '\n';

    for (long long maxD : maxDenominators) {
        Fraction approx = approximateReal(piFrac, maxD);
        double err = std::abs(piFrac - approx.toDouble());
        std::string fracStr = std::to_string(approx.num) + "/" + std::to_string(approx.den);
        std::cout << "  " << std::setw(14) << maxD
                  << std::setw(16) << fracStr
                  << std::setw(20) << std::fixed << std::setprecision(10) << approx.toDouble()
                  << std::setw(18) << std::scientific << std::setprecision(4) << err << '\n';
    }

    return 0;
}
