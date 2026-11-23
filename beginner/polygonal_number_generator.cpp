/*
 * ============================================================
 *            Polygonal (Figurate) Number Generator
 * ============================================================
 *
 * Description:
 *   The n-th s-gonal number is the number of dots needed to
 *   build a regular s-sided polygon pattern with n dots per side:
 *
 *     P(s, n) = ((s - 2) * n^2 - (s - 4) * n) / 2
 *
 *   s = 3: triangular  1, 3, 6, 10, 15, ...
 *   s = 4: square      1, 4, 9, 16, 25, ...
 *   s = 5: pentagonal  1, 5, 12, 22, 35, ...
 *   s = 6: hexagonal   1, 6, 15, 28, 45, ...
 *
 *   Inverse test: x is s-gonal iff
 *     n = (sqrt(8(s-2)x + (s-4)^2) + (s-4)) / (2(s-2))
 *   is a positive integer.
 *
 * Complexity: O(1) per term and per membership test.
 * ============================================================
 */

#include <iostream>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <string>

namespace polygonal {

    int64_t term(int64_t s, int64_t n) { return ((s - 2) * n * n - (s - 4) * n) / 2; }

    int64_t isqrt(int64_t v) {
        int64_t r = static_cast<int64_t>(std::sqrt(static_cast<double>(v)));
        while (r * r > v) --r;
        while ((r + 1) * (r + 1) <= v) ++r;
        return r;
    }

    // Returns n if x is the n-th s-gonal number, else 0
    int64_t index_of(int64_t s, int64_t x) {
        int64_t d = 8 * (s - 2) * x + (s - 4) * (s - 4);
        int64_t r = isqrt(d);
        if (r * r != d) return 0;
        int64_t num = r + (s - 4), den = 2 * (s - 2);
        return (num % den == 0) ? num / den : 0;
    }

} // namespace polygonal

int main() {
    using namespace polygonal;
    std::cout << "============================================================\n";
    std::cout << "            Polygonal (Figurate) Number Generator\n";
    std::cout << "============================================================\n\n";

    const std::string names[] = {"", "", "", "Triangular", "Square", "Pentagonal",
                                 "Hexagonal", "Heptagonal", "Octagonal"};
    for (int s = 3; s <= 8; ++s) {
        std::cout << std::left << std::setw(11) << names[s] << ":";
        for (int n = 1; n <= 10; ++n) std::cout << std::right << std::setw(5) << term(s, n);
        std::cout << "\n";
    }

    bool ok = true;
    for (int s = 3; s <= 12 && ok; ++s) {
        int64_t next = 1, n = 1;
        for (int64_t x = 1; x <= 20000; ++x) {
            bool is = (x == next);
            if (is) next = term(s, ++n);
            if ((index_of(s, x) != 0) != is) { ok = false; break; }
        }
    }
    std::cout << "\nInverse membership test vs generation (s=3..12, x<=20000): "
              << (ok ? "PASSED" : "FAILED") << "\n";

    std::cout << "\nNumbers <= 100000 that are triangular, pentagonal AND hexagonal:\n  ";
    for (int64_t n = 1; term(6, n) <= 100000; ++n) {
        int64_t h = term(6, n);
        if (index_of(3, h) && index_of(5, h)) std::cout << h << " ";
    }
    std::cout << "\n(Next one is 1533776805 - Project Euler #45.)\n";
    return 0;
}
