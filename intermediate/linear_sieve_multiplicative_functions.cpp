/*
 * ============================================================
 *     Linear Sieve for Multiplicative Functions
 *     (Euler phi, Mobius mu, divisor count d, divisor sum sigma)
 * ============================================================
 *
 * Description:
 *   The linear (Euler) sieve visits every composite exactly
 *   once, via its SMALLEST prime factor. While doing so it can
 *   compute any multiplicative function f in O(N) total, using:
 *     - for i and prime p with p not dividing i:  f(i p) = f(i) f(p)
 *     - for p | i (p = spf(i)): handle the prime-power part
 *
 *   phi:   phi(i p) = phi(i) * p          if p | i
 *          phi(i p) = phi(i) * (p - 1)    otherwise
 *   mu:    mu(i p)  = 0                   if p | i
 *          mu(i p)  = -mu(i)              otherwise
 *   d, sigma need the exponent / power of the smallest prime,
 *   tracked in cnt[] (exponent) and pw[] (p^exponent).
 *
 * Complexity: O(N) time, O(N) space.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>

namespace linear_sieve {

    struct Tables {
        std::vector<int> primes, spf, phi, mu, d, cnt;
        std::vector<long long> sigma, pw;
    };

    Tables build(int n) {
        Tables t;
        t.spf.assign(n + 1, 0); t.phi.assign(n + 1, 0); t.mu.assign(n + 1, 0);
        t.d.assign(n + 1, 0); t.cnt.assign(n + 1, 0);
        t.sigma.assign(n + 1, 0); t.pw.assign(n + 1, 0);
        t.phi[1] = t.mu[1] = t.d[1] = 1; t.sigma[1] = 1;

        for (int i = 2; i <= n; ++i) {
            if (!t.spf[i]) {
                t.spf[i] = i; t.primes.push_back(i);
                t.phi[i] = i - 1; t.mu[i] = -1; t.d[i] = 2; t.cnt[i] = 1;
                t.sigma[i] = i + 1; t.pw[i] = i;
            }
            for (int p : t.primes) {
                long long ip = 1LL * i * p;
                if (p > t.spf[i] || ip > n) break;
                int k = static_cast<int>(ip);
                t.spf[k] = p;
                if (p == t.spf[i]) {          // p divides i
                    t.phi[k] = t.phi[i] * p;
                    t.mu[k] = 0;
                    t.cnt[k] = t.cnt[i] + 1;
                    t.d[k] = t.d[i] / (t.cnt[i] + 1) * (t.cnt[k] + 1);
                    t.pw[k] = t.pw[i] * p;
                    // sigma(k) = sigma(i / pw_i) * (1 + p + ... + p^cnt_k)
                    long long rest = i / t.pw[i];
                    long long geo = (t.pw[k] * p - 1) / (p - 1);
                    t.sigma[k] = t.sigma[rest] * geo;
                } else {                      // coprime
                    t.phi[k] = t.phi[i] * (p - 1);
                    t.mu[k] = -t.mu[i];
                    t.cnt[k] = 1;
                    t.d[k] = t.d[i] * 2;
                    t.pw[k] = p;
                    t.sigma[k] = t.sigma[i] * (p + 1);
                }
            }
        }
        return t;
    }

} // namespace linear_sieve

int main() {
    std::cout << "============================================================\n";
    std::cout << "        Linear Sieve for Multiplicative Functions\n";
    std::cout << "============================================================\n\n";

    const int N = 200000;
    auto t = linear_sieve::build(N);

    std::cout << std::setw(5) << "n" << std::setw(6) << "spf" << std::setw(7) << "phi"
              << std::setw(5) << "mu" << std::setw(5) << "d" << std::setw(8) << "sigma" << "\n";
    for (int n = 1; n <= 20; ++n)
        std::cout << std::setw(5) << n << std::setw(6) << t.spf[n] << std::setw(7) << t.phi[n]
                  << std::setw(5) << t.mu[n] << std::setw(5) << t.d[n] << std::setw(8) << t.sigma[n] << "\n";

    // Brute force verification for n <= 3000
    bool ok = true;
    for (int n = 1; n <= 3000 && ok; ++n) {
        int dc = 0; long long ds = 0; int ph = 0;
        for (int k = 1; k <= n; ++k) {
            if (n % k == 0) { ++dc; ds += k; }
            int a = n, b = k; while (b) { int r = a % b; a = b; b = r; }
            if (a == 1) ++ph;
        }
        int m = n, mu = 1;
        for (int p = 2; p * p <= m; ++p)
            if (m % p == 0) { m /= p; if (m % p == 0) { mu = 0; break; } mu = -mu; }
        if (mu != 0 && m > 1) mu = -mu;
        if (dc != t.d[n] || ds != t.sigma[n] || ph != t.phi[n] || mu != t.mu[n]) ok = false;
    }
    std::cout << "\nBrute-force check (n <= 3000): " << (ok ? "PASSED" : "FAILED") << "\n";

    long long mertens = 0;
    for (int n = 1; n <= N; ++n) mertens += t.mu[n];
    std::cout << "pi(" << N << ") = " << t.primes.size() << ", Mertens M(" << N << ") = " << mertens << "\n";
    return 0;
}
