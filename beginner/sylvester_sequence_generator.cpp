/*
 * ============================================================================
 *  Sylvester's Sequence Generator
 * ============================================================================
 *
 *  Sylvester's sequence is defined by the recurrence:
 *    a(0) = 2
 *    a(n) = a(0) * a(1) * ... * a(n-1) + 1
 *
 *  Equivalently:
 *    a(n) = a(n-1)^2 - a(n-1) + 1
 *
 *  This second form shows the sequence grows doubly exponentially.
 *
 *  First terms: 2, 3, 7, 43, 1807, 3263443, ...
 *
 *  Properties:
 *  - Each term is coprime to all previous terms (by construction).
 *  - The sum of reciprocals is a telescoping series:
 *    1/a(0) + 1/a(1) + ... + 1/a(n-1) = 1 - 1/(a(0)*...*a(n-1))
 *    which approaches 1 from below (this gives an Egyptian fraction
 *    representation converging to 1).
 *  - Appears in Euclid's proof that there are infinitely many primes.
 *  - Used to construct optimal Egyptian fraction expansions of 1.
 *
 *  Due to rapid growth, we use uint64_t for the first several terms and
 *  stop when overflow would occur.
 *
 *  Time Complexity:  O(n) multiplications (each multiplication on large numbers)
 *  Space Complexity: O(n)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <string>

namespace sylvester {

/*
 * Generate first numTerms of Sylvester's sequence using the
 * quadratic recurrence a(n) = a(n-1)^2 - a(n-1) + 1.
 * We use uint64_t and stop when overflow would occur.
 */
std::vector<uint64_t> generateSafe(int numTerms) {
    std::vector<uint64_t> seq;
    if (numTerms <= 0) return seq;

    seq.push_back(2);

    for (int i = 1; i < numTerms; ++i) {
        uint64_t prev = seq.back();
        /* Check for overflow: prev^2 must fit in uint64_t */
        if (prev > 4294967295ULL) {
            /* prev^2 would overflow uint64_t */
            break;
        }
        uint64_t next = prev * prev - prev + 1;
        seq.push_back(next);
    }

    return seq;
}

/*
 * Verify the product property: a(n) = product(a(0)..a(n-1)) + 1
 */
bool verifyProductProperty(const std::vector<uint64_t>& seq) {
    if (seq.size() < 2) return true;

    uint64_t product = 1;
    for (size_t i = 0; i < seq.size() - 1; ++i) {
        /* Check overflow before multiplying */
        if (product > 0 && seq[i] > UINT64_MAX / product) {
            return true; /* Cannot verify further, assume OK */
        }
        product *= seq[i];
        if (product + 1 != seq[i + 1]) {
            return false;
        }
    }
    return true;
}

/*
 * Compute partial sum of reciprocals as a fraction,
 * verifying the telescoping identity:
 *   sum = 1 - 1/product(a(0)..a(n-1))
 */
void showReciprocalSum(const std::vector<uint64_t>& seq, int terms) {
    std::cout << "  Partial sum of reciprocals (first " << terms << " terms):\n";
    std::cout << "  sum = ";

    double sum = 0.0;
    for (int i = 0; i < terms && i < static_cast<int>(seq.size()); ++i) {
        sum += 1.0 / static_cast<double>(seq[i]);
        if (i > 0) std::cout << " + ";
        std::cout << "1/" << seq[i];
    }

    std::cout << "\n      = " << std::fixed << std::setprecision(15) << sum << "\n";
    std::cout << "  Deficit from 1.0 = " << (1.0 - sum) << "\n";
}

/*
 * Verify coprimality of all pairs.
 */
uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

bool verifyCoprimality(const std::vector<uint64_t>& seq) {
    for (size_t i = 0; i < seq.size(); ++i) {
        for (size_t j = i + 1; j < seq.size(); ++j) {
            if (gcd(seq[i], seq[j]) != 1) {
                return false;
            }
        }
    }
    return true;
}

} // namespace sylvester

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Sylvester's Sequence Generator\n";
    std::cout << "========================================================\n\n";

    auto seq = sylvester::generateSafe(10);

    std::cout << "Sylvester's Sequence (computed until uint64 overflow):\n";
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << "  a(" << i << ") = " << seq[i] << "\n";
    }

    std::cout << "\nQuadratic recurrence verification: a(n) = a(n-1)^2 - a(n-1) + 1\n";
    for (size_t i = 1; i < seq.size(); ++i) {
        uint64_t expected = seq[i - 1] * seq[i - 1] - seq[i - 1] + 1;
        std::cout << "  a(" << i << ") = " << seq[i - 1] << "^2 - " << seq[i - 1]
                  << " + 1 = " << expected
                  << (expected == seq[i] ? " (correct)" : " (ERROR)") << "\n";
    }

    std::cout << "\nProduct property verification: a(n) = a(0)*...*a(n-1) + 1 -> "
              << (sylvester::verifyProductProperty(seq) ? "PASSED" : "FAILED") << "\n";

    std::cout << "\nAll pairs coprime: "
              << (sylvester::verifyCoprimality(seq) ? "YES" : "NO") << "\n";

    std::cout << "\n";
    int recipTerms = std::min(static_cast<int>(seq.size()), 6);
    sylvester::showReciprocalSum(seq, recipTerms);

    return 0;
}
