/*
 * ============================================================
 *                   Benford's Law Checker
 * ============================================================
 *
 * Description:
 *   Benford's Law (First-Digit Law) states that in many
 *   naturally occurring sets of numerical data, the leading
 *   digit d (1-9) appears with probability:
 *
 *     P(d) = log10(1 + 1/d)
 *
 *   This means digit 1 appears ~30.1% of the time, digit 2
 *   about 17.6%, and so on, with digit 9 appearing only ~4.6%.
 *
 *   This program analyzes various number sequences (powers
 *   of 2, Fibonacci numbers, factorials) to demonstrate that
 *   they follow Benford's Law, and computes a chi-squared
 *   goodness-of-fit statistic.
 *
 * Mathematical Formulation:
 *   Expected frequency for leading digit d:
 *     P(d) = log10(1 + 1/d) for d in {1, 2, ..., 9}
 *
 *   Chi-squared statistic:
 *     chi2 = sum_{d=1}^{9} (observed_d - expected_d)^2 / expected_d
 *
 *   With 8 degrees of freedom, chi2 < 15.51 at 5% significance
 *   indicates a good fit.
 *
 * Complexity:
 *   Time:  O(N * D) where N is count of numbers, D is digit count
 *   Space: O(N) for storing the sequence
 *
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <array>

namespace benfords_law {

    // Expected Benford probabilities for digits 1-9
    std::array<double, 9> benford_expected() {
        std::array<double, 9> prob{};
        for (int d = 1; d <= 9; ++d) {
            prob[d - 1] = std::log10(1.0 + 1.0 / d);
        }
        return prob;
    }

    // Extract leading digit from a positive number string
    int leading_digit(const std::string& num_str) {
        for (char c : num_str) {
            if (c >= '1' && c <= '9') return c - '0';
        }
        return 0; // should not happen for positive numbers
    }

    // Generate powers of 2 as strings (using double for large ones)
    std::vector<std::string> generate_powers_of_2(int count) {
        std::vector<std::string> results;
        // Use logarithmic approach for leading digit extraction
        for (int i = 1; i <= count; ++i) {
            double log_val = i * std::log10(2.0);
            double frac = log_val - std::floor(log_val);
            double leading = std::pow(10.0, frac);
            results.push_back(std::to_string(static_cast<int>(leading)));
        }
        return results;
    }

    // Generate Fibonacci numbers (leading digits via Binet / log approach)
    std::vector<std::string> generate_fibonacci_leading(int count) {
        std::vector<std::string> results;
        double phi = (1.0 + std::sqrt(5.0)) / 2.0;
        double log_phi = std::log10(phi);
        double log_sqrt5 = std::log10(std::sqrt(5.0));

        // F(1) = 1, F(2) = 1
        results.push_back("1");
        results.push_back("1");

        for (int n = 3; n <= count; ++n) {
            // F(n) ~ phi^n / sqrt(5)
            double log_fn = n * log_phi - log_sqrt5;
            double frac = log_fn - std::floor(log_fn);
            double leading = std::pow(10.0, frac);
            results.push_back(std::to_string(static_cast<int>(leading)));
        }
        return results;
    }

    // Generate factorial leading digits via Stirling / log-gamma
    std::vector<std::string> generate_factorial_leading(int count) {
        std::vector<std::string> results;
        double log_fact = 0.0;
        for (int n = 1; n <= count; ++n) {
            log_fact += std::log10(static_cast<double>(n));
            double frac = log_fact - std::floor(log_fact);
            double leading = std::pow(10.0, frac);
            results.push_back(std::to_string(static_cast<int>(leading)));
        }
        return results;
    }

    // Count leading digit frequencies
    std::array<int, 9> count_leading_digits(const std::vector<std::string>& numbers) {
        std::array<int, 9> counts{};
        for (const auto& num : numbers) {
            int d = leading_digit(num);
            if (d >= 1 && d <= 9) {
                counts[d - 1]++;
            }
        }
        return counts;
    }

    // Compute chi-squared statistic
    double chi_squared(const std::array<int, 9>& observed, int total) {
        auto expected = benford_expected();
        double chi2 = 0.0;
        for (int i = 0; i < 9; ++i) {
            double exp_count = expected[i] * total;
            double diff = observed[i] - exp_count;
            chi2 += (diff * diff) / exp_count;
        }
        return chi2;
    }

    // Print analysis table
    void print_analysis(const std::string& title,
                        const std::array<int, 9>& observed,
                        int total) {
        auto expected = benford_expected();
        double chi2 = chi_squared(observed, total);

        std::cout << "\n--- " << title << " (N = " << total << ") ---\n";
        std::cout << std::setw(7) << "Digit"
                  << std::setw(12) << "Observed"
                  << std::setw(12) << "Obs %"
                  << std::setw(12) << "Benford %"
                  << std::setw(12) << "Diff %" << "\n";
        std::cout << std::string(55, '-') << "\n";

        for (int d = 1; d <= 9; ++d) {
            double obs_pct = 100.0 * observed[d - 1] / total;
            double exp_pct = 100.0 * expected[d - 1];

            std::cout << std::setw(7) << d
                      << std::setw(12) << observed[d - 1]
                      << std::setw(11) << std::fixed << std::setprecision(1) << obs_pct << "%"
                      << std::setw(11) << exp_pct << "%"
                      << std::setw(11) << std::showpos << (obs_pct - exp_pct)
                      << "%" << std::noshowpos << "\n";
        }

        std::cout << "\nChi-squared = " << std::setprecision(3) << chi2
                  << " (critical value at 5%: 15.51, df=8)\n";
        std::cout << "Fit: " << (chi2 < 15.51 ? "GOOD (follows Benford's Law)"
                                               : "POOR (does not follow Benford's Law)") << "\n";
    }

} // namespace benfords_law

int main() {
    using namespace benfords_law;

    std::cout << "============================================================\n";
    std::cout << "                Benford's Law Checker\n";
    std::cout << "============================================================\n";
    std::cout << "\nBenford's Law predicts: P(d) = log10(1 + 1/d)\n";

    // Print theoretical distribution
    auto expected = benford_expected();
    std::cout << "\nTheoretical distribution:\n";
    for (int d = 1; d <= 9; ++d) {
        std::cout << "  P(" << d << ") = " << std::fixed << std::setprecision(4)
                  << expected[d - 1] << " (" << std::setprecision(1)
                  << (100.0 * expected[d - 1]) << "%)\n";
    }

    const int N = 10000;

    // 1. Powers of 2
    {
        auto nums = generate_powers_of_2(N);
        auto counts = count_leading_digits(nums);
        print_analysis("Powers of 2", counts, N);
    }

    // 2. Fibonacci numbers
    {
        auto nums = generate_fibonacci_leading(N);
        auto counts = count_leading_digits(nums);
        print_analysis("Fibonacci Numbers", counts, N);
    }

    // 3. Factorials
    {
        auto nums = generate_factorial_leading(N);
        auto counts = count_leading_digits(nums);
        print_analysis("Factorials (n!)", counts, N);
    }

    // Counter-example: uniform distribution 1..9999
    {
        std::vector<std::string> uniform;
        for (int i = 1; i <= N; ++i) {
            uniform.push_back(std::to_string(i));
        }
        auto counts = count_leading_digits(uniform);
        int total = static_cast<int>(uniform.size());
        print_analysis("Uniform 1..10000 (counter-example)", counts, total);
    }

    std::cout << "\n============================================================\n";
    std::cout << "  Powers of 2, Fibonacci, and Factorials closely follow\n";
    std::cout << "  Benford's Law, while uniform sequences do not.\n";
    std::cout << "============================================================\n";

    return 0;
}
