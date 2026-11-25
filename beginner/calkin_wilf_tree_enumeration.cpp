/*
 * ============================================================
 *           Calkin-Wilf Tree Rational Enumeration
 * ============================================================
 *
 * Description:
 *   The Calkin-Wilf tree (2000) is a binary tree whose vertices
 *   bijectively enumerate all positive rational numbers in
 *   reduced form:
 *     - Root is 1/1.
 *     - Vertex a/b has left child a/(a+b) and right child (a+b)/b.
 *
 *   Breadth-first traversal gives the sequence:
 *     1/1, 1/2, 2/1, 1/3, 3/2, 2/3, 3/1, 1/4, 4/3, 3/5, 5/2, ...
 *   where the numerator of the n-th fraction equals the
 *   denominator of the (n-1)-th fraction:
 *     q(n) = fusc(n) / fusc(n + 1)
 *
 *   Moshe Newman's direct iteration formula jumps from q to the
 *   next rational in BFS order in O(1) arithmetic operations:
 *     next(q) = 1 / (2 * floor(q) + 1 - q)
 *
 * Complexity:
 *   Next rational: O(1)
 *   k-th rational via binary bits of k: O(log k)
 *   Index of p/q via Euclidean step run-lengths: O(log(p + q))
 * ============================================================
 */

#include <iostream>
#include <cstdint>
#include <numeric>
#include <utility>
#include <vector>
#include <iomanip>

namespace calkin_wilf {

    using u64 = uint64_t;
    struct Frac { u64 p, q; };

    // Newman's recurrence: next(p/q) = 1 / (2*floor(p/q) + 1 - p/q)
    // = q / (2*(p/q)*q + q - p) = q / (2*(p - (p%q)) + q - p) = q / (p + q - 2*(p % q))
    Frac next_rational(Frac r) {
        return {r.q, r.p + r.q - 2 * (r.p % r.q)};
    }

    // k-th rational (1-indexed: k=1 -> 1/1) by reading binary digits of k from MSB-1 to LSB
    Frac kth_rational(u64 k) {
        u64 a = 1, b = 1;
        int msb = 63;
        while (msb > 0 && !((k >> msb) & 1)) --msb;
        for (int i = msb - 1; i >= 0; --i) {
            if ((k >> i) & 1) a += b;   // right child: (a+b)/b
            else              b += a;   // left child:  a/(a+b)
        }
        return {a, b};
    }

    // Inverse: find 1-based BFS index of reduced fraction a/b
    u64 index_of(u64 a, u64 b) {
        std::vector<int> bits;
        while (a != b) {
            if (a > b) { bits.push_back(1); a -= b; }
            else       { bits.push_back(0); b -= a; }
        }
        u64 k = 1;
        for (int i = static_cast<int>(bits.size()) - 1; i >= 0; --i)
            k = (k << 1) | bits[i];
        return k;
    }

} // namespace calkin_wilf

int main() {
    using namespace calkin_wilf;
    std::cout << "============================================================\n";
    std::cout << "           Calkin-Wilf Tree Rational Enumeration\n";
    std::cout << "============================================================\n\n";

    std::cout << "First 24 rationals in Calkin-Wilf BFS order:\n";
    Frac cur{1, 1};
    bool ok = true;
    for (u64 k = 1; k <= 24; ++k) {
        std::cout << std::setw(4) << cur.p << "/" << std::left << std::setw(3) << cur.q << std::right;
        if (k % 8 == 0) std::cout << "\n";
        Frac by_k = kth_rational(k);
        if (by_k.p != cur.p || by_k.q != cur.q || std::gcd(cur.p, cur.q) != 1) ok = false;
        if (index_of(cur.p, cur.q) != k) ok = false;
        cur = next_rational(cur);
    }

    // Verify 5000 steps of Newman's recurrence against bit-path evaluation and inverse
    cur = {1, 1};
    for (u64 k = 1; k <= 5000 && ok; ++k) {
        Frac f = kth_rational(k);
        if (f.p != cur.p || f.q != cur.q || index_of(f.p, f.q) != k) ok = false;
        cur = next_rational(cur);
    }
    std::cout << "\nNewman recurrence == binary path == inverse (k <= 5000): "
              << (ok ? "PASSED" : "FAILED") << "\n";

    Frac sample = kth_rational(1000000);
    std::cout << "1,000,000-th rational: " << sample.p << "/" << sample.q
              << " (index_of = " << index_of(sample.p, sample.q) << ")\n";
    return 0;
}
