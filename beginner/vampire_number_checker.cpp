/**
 * ============================================================================
 * Vampire Number Checker & Fang Factorization Analyzer
 * ============================================================================
 * 
 * Description:
 * A Vampire number (defined by Clifford A. Pickover in 1994) is a composite
 * natural number v with an even number of digits 2k that can be factored into
 * two positive integers x and y (called the "fangs"), each having exactly k digits,
 * satisfying:
 *   1. v = x * y
 *   2. The combined digits of x and y form an exact permutation (multiset match)
 *      of the decimal digits of v.
 *   3. x and y cannot both end with a trailing zero (to prevent trivial trailing
 *      zero extensions like 126000 = 210 * 600).
 * 
 * Examples:
 * - 1260 has 4 digits (k = 2):
 *     Fangs: 21 and 60 (both 2 digits)
 *     21 * 60 = 1260
 *     Digits of {2, 1} + {6, 0} = {0, 1, 2, 6} = Digits of 1260
 *     Trailed zeros: only one ends in zero. Valid!
 * 
 * Complete List of 4-Digit Vampire Numbers:
 *   1260 = 21 * 60
 *   1395 = 15 * 93
 *   1435 = 35 * 41
 *   1530 = 30 * 51
 *   1827 = 21 * 87
 *   2187 = 27 * 81
 *   6880 = 80 * 86
 * 
 * Time Complexity:
 * - O(sqrt(v)) to test all potential fang pairs for a number v.
 * - Space Complexity: O(log_10 v) auxiliary space.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <iomanip>

namespace vampire {

struct FangPair {
    int64_t x;
    int64_t y;
};

struct VampireResult {
    bool is_vampire;
    std::vector<FangPair> fangs;
};

/**
 * Checks whether the combined digits of x and y match the digits of v.
 */
bool digitsMatch(int64_t v, int64_t x, int64_t y) {
    int count_v[10] = {0};
    int count_xy[10] = {0};

    while (v > 0) {
        count_v[v % 10]++;
        v /= 10;
    }
    while (x > 0) {
        count_xy[x % 10]++;
        x /= 10;
    }
    while (y > 0) {
        count_xy[y % 10]++;
        y /= 10;
    }

    for (int d = 0; d < 10; ++d) {
        if (count_v[d] != count_xy[d]) {
            return false;
        }
    }
    return true;
}

/**
 * Tests if v is a vampire number and finds all valid fang pairs.
 */
VampireResult analyzeVampire(int64_t v) {
    VampireResult res{false, {}};
    if (v <= 0) return res;

    std::string s = std::to_string(v);
    int total_digits = static_cast<int>(s.length());

    // Vampire numbers must have an even number of digits (2*k)
    if (total_digits % 2 != 0) {
        return res;
    }

    int k = total_digits / 2;
    int64_t min_fang = 1;
    for (int i = 0; i < k - 1; ++i) min_fang *= 10;
    int64_t max_fang = min_fang * 10 - 1;

    for (int64_t x = min_fang; x * x <= v && x <= max_fang; ++x) {
        if (v % x == 0) {
            int64_t y = v / x;

            // y must also have exactly k digits
            if (y >= min_fang && y <= max_fang) {
                // Rule: Cannot both end with trailing zeroes
                if (!(x % 10 == 0 && y % 10 == 0)) {
                    if (digitsMatch(v, x, y)) {
                        res.is_vampire = true;
                        res.fangs.push_back({x, y});
                    }
                }
            }
        }
    }

    return res;
}

} // namespace vampire

int main() {
    std::cout << "========================================================\n";
    std::cout << " Vampire Number Checker & Fang Factorization Analyzer\n";
    std::cout << "========================================================\n\n";

    // Exhaustive search for all 4-digit vampire numbers [1000, 9999]
    std::cout << "Searching for all 4-digit Vampire Numbers [1000, 9999]:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(10) << "Number"
              << std::setw(20) << "Fangs (x * y)"
              << std::setw(24) << "Digit Verification\n";
    std::cout << "--------------------------------------------------------\n";

    int count_4digit = 0;
    for (int64_t n = 1000; n <= 9999; ++n) {
        auto res = vampire::analyzeVampire(n);
        if (res.is_vampire) {
            count_4digit++;
            for (const auto& pair : res.fangs) {
                std::string fang_str = std::to_string(pair.x) + " * " + std::to_string(pair.y);
                std::cout << std::setw(10) << n
                          << std::setw(20) << fang_str
                          << std::setw(24) << "EXACT PERMUTATION"
                          << "\n";
            }
        }
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Total 4-digit vampire numbers found: " << count_4digit << " (Expected: 7)\n\n";

    // Testing interesting 6-digit vampire numbers
    std::cout << "Testing Select 6-Digit Vampire Numbers:\n";
    std::cout << "--------------------------------------------------------\n";
    int64_t sample_6digit[] = {125460, 126000, 130788, 146520};

    for (int64_t v : sample_6digit) {
        auto res = vampire::analyzeVampire(v);
        std::cout << "Number: " << v << " -> ";
        if (res.is_vampire) {
            std::cout << "VAMPIRE (Fangs: ";
            for (size_t i = 0; i < res.fangs.size(); ++i) {
                std::cout << res.fangs[i].x << " * " << res.fangs[i].y;
                if (i + 1 < res.fangs.size()) std::cout << ", ";
            }
            std::cout << ")\n";
        } else {
            std::cout << "NOT VAMPIRE (e.g. trailing zero rule or non-matching fangs)\n";
        }
    }

    return 0;
}
