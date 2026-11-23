/*
 * ============================================================
 *        Duval's Algorithm: Lyndon Factorization
 * ============================================================
 *
 * Description:
 *   A Lyndon word is a non-empty string strictly smaller than
 *   all of its proper rotations (equivalently, all its proper
 *   suffixes). The Chen-Fox-Lyndon theorem states every string
 *   s has a UNIQUE factorization
 *       s = w1 w2 ... wk,   w1 >= w2 >= ... >= wk
 *   into Lyndon words.
 *
 *   Duval's algorithm computes it in linear time with O(1)
 *   extra memory using three pointers i <= j < k:
 *     - s[i..k) is a prefix of the form (w)^m w' (pre-Lyndon)
 *     - compare s[j] and s[k]:
 *         s[j] == s[k] -> extend periodic run  (j++, k++)
 *         s[j] <  s[k] -> s[i..k] becomes one Lyndon word (j = i)
 *         s[j] >  s[k] -> output copies of w (length k - j)
 *
 * Application: minimal cyclic rotation of a string, found by
 *   running Duval on s+s and taking the start of the last
 *   Lyndon word that begins in the first half.
 *
 * Complexity: O(N) time, O(1) extra space.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

namespace lyndon {

    std::vector<std::string> duval(const std::string& s) {
        std::vector<std::string> out;
        size_t n = s.size(), i = 0;
        while (i < n) {
            size_t j = i, k = i + 1;
            while (k < n && s[j] <= s[k]) {
                if (s[j] < s[k]) j = i; else ++j;
                ++k;
            }
            while (i <= j) {
                out.push_back(s.substr(i, k - j));
                i += k - j;
            }
        }
        return out;
    }

    size_t min_rotation(const std::string& str) {
        std::string s = str + str;
        size_t n = s.size(), i = 0, ans = 0;
        while (i < n / 2) {
            ans = i;
            size_t j = i, k = i + 1;
            while (k < n && s[j] <= s[k]) {
                if (s[j] < s[k]) j = i; else ++j;
                ++k;
            }
            while (i <= j) i += k - j;
        }
        return ans;
    }

    bool is_lyndon(const std::string& w) {
        for (size_t r = 1; r < w.size(); ++r)
            if (!(w < w.substr(r) + w.substr(0, r))) return false;
        return !w.empty();
    }

} // namespace lyndon

int main() {
    using namespace lyndon;
    std::cout << "============================================================\n";
    std::cout << "         Duval's Algorithm: Lyndon Factorization\n";
    std::cout << "============================================================\n\n";

    for (std::string s : {"banana", "abracadabra", "aaaa", "cbabcab", "abab", "zyxwv", "abaabaab"}) {
        auto f = duval(s);
        std::cout << s << " = ";
        for (size_t i = 0; i < f.size(); ++i) std::cout << (i ? " . " : "") << "(" << f[i] << ")";
        size_t r = min_rotation(s);
        std::cout << "   | min rotation: " << s.substr(r) + s.substr(0, r) << "\n";
    }

    std::mt19937 rng(5);
    bool ok = true;
    for (int t = 0; t < 3000 && ok; ++t) {
        int len = 1 + rng() % 14;
        std::string s;
        for (int i = 0; i < len; ++i) s += char('a' + rng() % 3);
        auto f = duval(s);
        std::string joined;
        for (size_t i = 0; i < f.size(); ++i) {
            joined += f[i];
            if (!is_lyndon(f[i])) ok = false;
            if (i && f[i - 1] < f[i]) ok = false;   // must be non-increasing
        }
        if (joined != s) ok = false;
        std::string best = s;
        for (int r = 1; r < len; ++r) best = std::min(best, s.substr(r) + s.substr(0, r));
        size_t r = min_rotation(s);
        if (s.substr(r) + s.substr(0, r) != best) ok = false;
    }
    std::cout << "\nRandom verification (3000 strings: factor validity + min rotation): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
