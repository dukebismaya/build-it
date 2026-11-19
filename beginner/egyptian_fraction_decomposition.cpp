/*
 * ============================================================
 *            Egyptian Fraction Decomposition
 * ============================================================
 *
 * Description:
 *   An Egyptian fraction is a representation of a rational
 *   number as a sum of distinct unit fractions (fractions
 *   with numerator 1). For example:
 *     5/6 = 1/2 + 1/3
 *     7/11 = 1/2 + 1/8 + 1/88
 *
 *   The greedy algorithm (Fibonacci-Sylvester) repeatedly
 *   subtracts the largest possible unit fraction from the
 *   remaining fraction until the numerator becomes zero.
 *
 * Algorithm (Greedy / Fibonacci-Sylvester):
 *   Given fraction p/q (0 < p/q <= 1):
 *   1. Find the smallest integer n such that 1/n <= p/q
 *      i.e., n = ceil(q / p)
 *   2. Output 1/n as part of the decomposition
 *   3. Compute remainder: p/q - 1/n = (p*n - q) / (q*n)
 *   4. Reduce the fraction and repeat from step 1
 *   5. Stop when p becomes 0
 *
 * Complexity:
 *   The number of terms is at most O(p) for input p/q,
 *   and in practice the algorithm terminates quickly.
 *   Each step: O(log(q)) for GCD computation.
 *
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <numeric>
#include <cstdint>
#include <iomanip>

namespace egyptian_fraction {

    using ll = int64_t;

    // Compute GCD
    ll gcd(ll a, ll b) {
        a = (a < 0) ? -a : a;
        b = (b < 0) ? -b : b;
        while (b) {
            a %= b;
            ll tmp = a;
            a = b;
            b = tmp;
        }
        return a;
    }

    // Decompose p/q into Egyptian fractions using greedy algorithm
    // Returns a vector of denominators of unit fractions
    std::vector<ll> decompose(ll p, ll q) {
        std::vector<ll> denominators;

        // Reduce fraction
        ll g = gcd(p, q);
        p /= g;
        q /= g;

        while (p > 0 && denominators.size() < 100) {
            // Smallest n such that 1/n <= p/q => n = ceil(q/p)
            ll n = (q + p - 1) / p;
            denominators.push_back(n);

            // p/q - 1/n = (p*n - q) / (q*n)
            p = p * n - q;
            q = q * n;

            // Reduce
            if (p > 0) {
                g = gcd(p, q);
                p /= g;
                q /= g;
            }
        }

        return denominators;
    }

    // Format as a sum of unit fractions string
    std::string format_decomposition(const std::vector<ll>& denoms) {
        std::string result;
        for (size_t i = 0; i < denoms.size(); ++i) {
            if (i > 0) result += " + ";
            result += "1/" + std::to_string(denoms[i]);
        }
        return result;
    }

    // Verify: check that the sum of 1/d_i equals p/q
    bool verify(ll p, ll q, const std::vector<ll>& denoms) {
        // Compute sum as exact fraction
        ll sum_num = 0;
        ll sum_den = 1;

        for (ll d : denoms) {
            // sum_num/sum_den + 1/d = (sum_num * d + sum_den) / (sum_den * d)
            sum_num = sum_num * d + sum_den;
            sum_den = sum_den * d;
            ll g = gcd(sum_num, sum_den);
            sum_num /= g;
            sum_den /= g;
        }

        // Compare with reduced p/q
        ll g = gcd(p, q);
        p /= g;
        q /= g;

        return sum_num == p && sum_den == q;
    }

} // namespace egyptian_fraction

int main() {
    using namespace egyptian_fraction;

    std::cout << "============================================================\n";
    std::cout << "          Egyptian Fraction Decomposition\n";
    std::cout << "       (Fibonacci-Sylvester Greedy Algorithm)\n";
    std::cout << "============================================================\n\n";

    // Test cases: various fractions
    struct TestCase {
        ll p, q;
    };

    std::vector<TestCase> tests = {
        {2, 3},     // 2/3 = 1/2 + 1/6
        {3, 4},     // 3/4 = 1/2 + 1/4
        {5, 6},     // 5/6 = 1/2 + 1/3
        {4, 7},     // 4/7
        {7, 11},    // 7/11
        {5, 13},    // 5/13
        {8, 15},    // 8/15
        {11, 17},   // 11/17
        {23, 37},   // 23/37
        {1, 1},     // 1/1 = 1/1
        {3, 7},     // 3/7
        {12, 13},   // 12/13 - interesting decomposition
    };

    for (const auto& tc : tests) {
        auto denoms = decompose(tc.p, tc.q);
        bool valid = verify(tc.p, tc.q, denoms);

        std::cout << std::setw(5) << tc.p << "/" << std::left << std::setw(4) << tc.q
                  << " = " << format_decomposition(denoms)
                  << "  [" << (valid ? "OK" : "FAIL") << "]"
                  << " (" << denoms.size() << " terms)\n";
        std::cout << std::right;
    }

    // Show a detailed step-by-step decomposition
    std::cout << "\n--- Step-by-step decomposition of 5/13 ---\n";
    {
        ll p = 5, q = 13;
        int step = 0;
        while (p > 0) {
            ll g = gcd(p, q);
            p /= g;
            q /= g;
            ll n = (q + p - 1) / p;
            std::cout << "Step " << ++step << ": "
                      << p << "/" << q << " -> subtract 1/" << n
                      << " -> remainder = ";
            ll new_p = p * n - q;
            ll new_q = q * n;
            if (new_p == 0) {
                std::cout << "0 (done)\n";
            } else {
                ll gg = gcd(new_p, new_q);
                std::cout << (new_p / gg) << "/" << (new_q / gg) << "\n";
            }
            p = new_p;
            q = new_q;
        }
    }

    std::cout << "\n============================================================\n";
    std::cout << "  Every positive rational number can be expressed as a\n";
    std::cout << "  finite sum of distinct unit fractions (Erdos-Straus\n";
    std::cout << "  conjecture relates to 4/n specifically).\n";
    std::cout << "============================================================\n";

    return 0;
}
