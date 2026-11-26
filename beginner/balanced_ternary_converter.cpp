/*
 * ============================================================
 *            Balanced Ternary Number System Converter
 * ============================================================
 *
 * Description:
 *   Balanced ternary is a positional base-3 numeral system using
 *   the symmetric trits {-1, 0, +1}, written as {'T', '0', '1'}:
 *     n = sum_{k >= 0} d_k * 3^k,   d_k in {-1, 0, +1}
 *
 *   Properties:
 *     - Every integer (positive, negative, or zero) has a UNIQUE
 *       representation without a separate minus sign.
 *     - Negation is achieved by swapping '1' <-> 'T'.
 *     - Truncating after any digit rounds to the nearest multiple.
 *     - Classic application: Bachet's 4-weight balance problem
 *       (weighing any integer load 1..40 on a two-pan balance
 *       with weights 1, 3, 9, 27).
 *
 *   Conversion algorithm (n -> balanced ternary):
 *     While n != 0:
 *       rem = ((n % 3) + 3) % 3
 *       if rem == 0: append '0', n /= 3
 *       if rem == 1: append '1', n = (n - 1) / 3
 *       if rem == 2: append 'T', n = (n + 1) / 3
 *
 * Complexity:
 *   O(log_3 |N|) trits for conversion and arithmetic.
 * ============================================================
 */

#include <iostream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <iomanip>

namespace balanced_ternary {

    using i64 = int64_t;

    std::string encode(i64 n) {
        if (n == 0) return "0";
        std::string s;
        while (n != 0) {
            i64 r = ((n % 3) + 3) % 3;
            if (r == 0)      { s.push_back('0'); n /= 3; }
            else if (r == 1) { s.push_back('1'); n = (n - 1) / 3; }
            else             { s.push_back('T'); n = (n + 1) / 3; }
        }
        std::reverse(s.begin(), s.end());
        return s;
    }

    i64 decode(const std::string& s) {
        i64 v = 0;
        for (char c : s) {
            int trit = (c == '1') ? 1 : (c == 'T') ? -1 : 0;
            v = v * 3 + trit;
        }
        return v;
    }

    std::string negate(std::string s) {
        for (char& c : s) {
            if (c == '1') c = 'T';
            else if (c == 'T') c = '1';
        }
        return s;
    }

    // Trit-by-trit addition without converting to decimal
    std::string add(const std::string& a, const std::string& b) {
        std::string out;
        int i = static_cast<int>(a.size()) - 1;
        int j = static_cast<int>(b.size()) - 1;
        int carry = 0;
        auto val = [](char c) { return c == '1' ? 1 : c == 'T' ? -1 : 0; };
        while (i >= 0 || j >= 0 || carry != 0) {
            int sum = carry;
            if (i >= 0) sum += val(a[i--]);
            if (j >= 0) sum += val(b[j--]);
            // Decompose sum in [-3, +3] into next_carry * 3 + digit, digit in {-1, 0, 1}
            int nc = 0;
            if (sum >= 2)       { sum -= 3; nc = 1; }
            else if (sum <= -2) { sum += 3; nc = -1; }
            out.push_back(sum == 1 ? '1' : sum == -1 ? 'T' : '0');
            carry = nc;
        }
        while (out.size() > 1 && out.back() == '0') out.pop_back();
        std::reverse(out.begin(), out.end());
        return out;
    }

} // namespace balanced_ternary

int main() {
    using namespace balanced_ternary;
    std::cout << "============================================================\n";
    std::cout << "            Balanced Ternary Number System Converter\n";
    std::cout << "============================================================\n\n";

    std::cout << "Integers -13..+13 in balanced ternary (3 trits):\n";
    for (int n = -13; n <= 13; ++n) {
        std::cout << std::setw(4) << n << " = " << std::setw(4) << encode(n);
        if ((n + 14) % 5 == 0) std::cout << "\n";
    }
    std::cout << "\n\nBachet's 4-weight puzzle (weights 1, 3, 9, 27) for loads 1..15:\n";
    for (int w = 1; w <= 15; ++w) {
        std::string s = encode(w);
        std::cout << "  Load " << std::setw(2) << w << " (" << std::setw(4) << s << "): ";
        int p = 1;
        for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i, p *= 3) {
            if (s[i] == '1') std::cout << "+" << p << " ";
            else if (s[i] == 'T') std::cout << "-" << p << " ";
        }
        std::cout << "\n";
    }

    bool ok = true;
    for (i64 a = -200; a <= 200 && ok; ++a) {
        std::string sa = encode(a);
        if (decode(sa) != a || decode(negate(sa)) != -a) ok = false;
        for (i64 b = -50; b <= 50 && ok; ++b) {
            std::string sb = encode(b);
            std::string sum = add(sa, sb);
            if (decode(sum) != a + b || sum != encode(a + b)) ok = false;
        }
    }
    std::cout << "\nRound-trip encode/decode, negation & trit addition (-200..200): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
