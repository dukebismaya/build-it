/*
 * ============================================================================
 *  Van der Corput Sequence Generator (Low-Discrepancy / Quasi-Random)
 * ============================================================================
 *
 *  The Van der Corput sequence is the simplest one-dimensional low-discrepancy
 *  (quasi-random) sequence. For a given base b, the n-th element is obtained
 *  by reflecting the base-b digits of n about the decimal point.
 *
 *  For base 2:
 *    n = 1 (1 in binary)    -> 0.1    = 1/2
 *    n = 2 (10)             -> 0.01   = 1/4
 *    n = 3 (11)             -> 0.11   = 3/4
 *    n = 4 (100)            -> 0.001  = 1/8
 *    n = 5 (101)            -> 0.101  = 5/8
 *
 *  Properties:
 *  - Equidistributed: for any subinterval [a,b], the fraction of first N
 *    terms in [a,b] approaches (b - a) as N grows.
 *  - Discrepancy is O(log N / N), much better than pseudo-random O(1/sqrt(N)).
 *  - Used in quasi-Monte Carlo integration, numerical sampling, and
 *    low-discrepancy point set generation (Halton sequences use multiple bases).
 *
 *  Time Complexity:  O(log n) per element (number of digits in base b)
 *  Space Complexity: O(1) per element
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <cmath>
#include <algorithm>

namespace quasi_random {

/*
 * Compute the n-th Van der Corput number in the given base.
 * Reflects the digits of n about the radix point.
 */
double vanDerCorput(int64_t n, int base) {
    double result = 0.0;
    double denom = 1.0;

    while (n > 0) {
        denom *= base;
        result += static_cast<double>(n % base) / denom;
        n /= base;
    }

    return result;
}

/*
 * Generate first count elements of the Van der Corput sequence in given base.
 */
std::vector<double> generateSequence(int count, int base) {
    std::vector<double> seq;
    seq.reserve(count);
    for (int i = 1; i <= count; ++i) {
        seq.push_back(vanDerCorput(i, base));
    }
    return seq;
}

/*
 * Compute star discrepancy estimate (sup over [0,x] intervals).
 * D*_N = sup_{0 < x <= 1} |A([0,x), N)/N - x|
 * where A([0,x), N) counts points in [0, x).
 */
double starDiscrepancy(const std::vector<double>& points) {
    int n = static_cast<int>(points.size());
    std::vector<double> sorted = points;
    std::sort(sorted.begin(), sorted.end());

    double maxDisc = 0.0;
    for (int i = 0; i < n; ++i) {
        /* Check discrepancy just before and at sorted[i] */
        double expected = sorted[i];
        double actual = static_cast<double>(i) / n;
        maxDisc = std::max(maxDisc, std::abs(expected - actual));

        actual = static_cast<double>(i + 1) / n;
        maxDisc = std::max(maxDisc, std::abs(expected - actual));
    }
    /* Also check at x = 1 */
    maxDisc = std::max(maxDisc, std::abs(1.0 - 1.0));

    return maxDisc;
}

/*
 * Simple quasi-Monte Carlo integration estimate of integral of f(x) on [0,1].
 * Uses Van der Corput points instead of random points.
 */
double quasiMonteCarloIntegral(int numPoints, int base,
                                double (*f)(double)) {
    double sum = 0.0;
    for (int i = 1; i <= numPoints; ++i) {
        double x = vanDerCorput(i, base);
        sum += f(x);
    }
    return sum / numPoints;
}

} // namespace quasi_random

/* Test functions for integration */
static double f_square(double x) { return x * x; }
static double f_sin(double x) { return std::sin(x); }
static double f_exp(double x) { return std::exp(x); }

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Van der Corput Sequence (Low-Discrepancy Generator)\n";
    std::cout << "========================================================\n\n";

    /* Show first 16 elements in bases 2, 3, 5 */
    int bases[] = {2, 3, 5};
    for (int base : bases) {
        std::cout << "Base " << base << " (first 16 elements):\n  ";
        auto seq = quasi_random::generateSequence(16, base);
        for (size_t i = 0; i < seq.size(); ++i) {
            std::cout << std::fixed << std::setprecision(4) << seq[i];
            if (i + 1 < seq.size()) std::cout << ", ";
        }
        std::cout << "\n\n";
    }

    /* Discrepancy comparison: Van der Corput vs "uniform" */
    std::cout << "Star Discrepancy Comparison (base 2):\n";
    std::cout << std::setw(10) << "N" << std::setw(16) << "VdC Disc."
              << std::setw(16) << "log(N)/N" << "\n";
    std::cout << "------------------------------------------\n";

    for (int n : {10, 50, 100, 500, 1000}) {
        auto seq = quasi_random::generateSequence(n, 2);
        double disc = quasi_random::starDiscrepancy(seq);
        double theoretical = std::log(static_cast<double>(n)) / n;

        std::cout << std::setw(10) << n
                  << std::setw(16) << std::fixed << std::setprecision(6) << disc
                  << std::setw(16) << theoretical << "\n";
    }

    /* Quasi-Monte Carlo integration */
    std::cout << "\nQuasi-Monte Carlo Integration on [0, 1]:\n";
    std::cout << std::setw(12) << "Function" << std::setw(12) << "N"
              << std::setw(14) << "QMC Result" << std::setw(14) << "Exact"
              << std::setw(14) << "Error" << "\n";
    std::cout << "--------------------------------------------------------------\n";

    struct IntTest { const char* name; double (*fn)(double); double exact; };
    IntTest tests[] = {
        {"x^2",   f_square, 1.0 / 3.0},
        {"sin(x)", f_sin,   1.0 - std::cos(1.0)},
        {"exp(x)", f_exp,   std::exp(1.0) - 1.0}
    };

    for (auto& t : tests) {
        for (int n : {100, 1000}) {
            double result = quasi_random::quasiMonteCarloIntegral(n, 2, t.fn);
            double err = std::abs(result - t.exact);
            std::cout << std::setw(12) << t.name << std::setw(12) << n
                      << std::setw(14) << std::setprecision(8) << result
                      << std::setw(14) << t.exact
                      << std::setw(14) << std::scientific << std::setprecision(3) << err << "\n";
        }
    }

    return 0;
}
