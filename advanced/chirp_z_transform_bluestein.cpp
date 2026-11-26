/*
 * ============================================================
 *       Bluestein's Chirp Z-Transform (Arbitrary-Length FFT)
 * ============================================================
 *
 * Description:
 *   Standard Cooley-Tukey FFT requires the transform length N
 *   to be a power of 2 (or smooth). Bluestein's algorithm (1970)
 *   computes the Discrete Fourier Transform (DFT) of ANY length
 *   N (including prime N!) in O(N log N) time by rewriting the
 *   DFT exponent product using the chirp identity:
 *     n * k = (n^2 + k^2 - (k - n)^2) / 2
 *
 *   Then the DFT sum:
 *     X[k] = sum_{n=0}^{N-1} x[n] * W^{n k},   W = exp(-2*pi*i / N)
 *   becomes a linear convolution:
 *     X[k] = W^{k^2 / 2} * sum_{n=0}^{N-1} (x[n] * W^{n^2 / 2}) * W^{-(k - n)^2 / 2}
 *   which is evaluated by a single power-of-two radix-2 FFT of
 *   size M >= 2N - 1!
 *
 *   More generally, Bluestein's Chirp Z-Transform evaluates the
 *   z-transform along any geometric contour z_k = A * W^{-k}.
 *
 * Complexity:
 *   Time:  O(N log N) for any N (prime or composite)
 *   Space: O(N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <random>
#include <iomanip>

namespace bluestein {

    using cd = std::complex<double>;
    const double PI = std::acos(-1.0);

    void radix2_fft(std::vector<cd>& a, bool invert) {
        int n = static_cast<int>(a.size());
        for (int i = 1, j = 0; i < n; ++i) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) j ^= bit;
            j ^= bit;
            if (i < j) std::swap(a[i], a[j]);
        }
        for (int len = 2; len <= n; len <<= 1) {
            double ang = 2.0 * PI / len * (invert ? 1.0 : -1.0);
            cd wlen(std::cos(ang), std::sin(ang));
            for (int i = 0; i < n; i += len) {
                cd w(1.0, 0.0);
                for (int j = 0; j < len / 2; ++j) {
                    cd u = a[i + j], v = a[i + j + len / 2] * w;
                    a[i + j] = u + v;
                    a[i + j + len / 2] = u - v;
                    w *= wlen;
                }
            }
        }
        if (invert)
            for (cd& x : a) x /= n;
    }

    // Arbitrary-length DFT (or inverse DFT) via Bluestein's Chirp-Z algorithm
    std::vector<cd> dft_arbitrary(const std::vector<cd>& x, bool invert = false) {
        int n = static_cast<int>(x.size());
        if (n <= 1) return x;
        int m = 1;
        while (m < 2 * n - 1) m <<= 1;

        double sign = invert ? 1.0 : -1.0;
        std::vector<cd> chirp(n);
        for (int k = 0; k < n; ++k) {
            double angle = sign * PI * (1LL * k * k % (2LL * n)) / n;
            chirp[k] = cd(std::cos(angle), std::sin(angle));
        }

        std::vector<cd> A(m, 0.0), B(m, 0.0);
        for (int k = 0; k < n; ++k) A[k] = x[k] * chirp[k];
        B[0] = std::conj(chirp[0]);
        for (int k = 1; k < n; ++k) {
            B[k] = std::conj(chirp[k]);
            B[m - k] = std::conj(chirp[k]); // wrap negative lags -(n-1)..-1
        }

        radix2_fft(A, false);
        radix2_fft(B, false);
        for (int i = 0; i < m; ++i) A[i] *= B[i];
        radix2_fft(A, true);

        std::vector<cd> X(n);
        for (int k = 0; k < n; ++k) {
            X[k] = A[k] * chirp[k];
            if (invert) X[k] /= static_cast<double>(n);
        }
        return X;
    }

    std::vector<cd> naive_dft(const std::vector<cd>& x) {
        int n = static_cast<int>(x.size());
        std::vector<cd> X(n, 0.0);
        for (int k = 0; k < n; ++k)
            for (int j = 0; j < n; ++j) {
                double ang = -2.0 * PI * (1LL * j * k % n) / n;
                X[k] += x[j] * cd(std::cos(ang), std::sin(ang));
            }
        return X;
    }

} // namespace bluestein

int main() {
    using namespace bluestein;
    std::cout << "============================================================\n";
    std::cout << "       Bluestein's Chirp Z-Transform (Arbitrary-Length FFT)\n";
    std::cout << "============================================================\n\n";

    std::mt19937 rng(2026);
    bool ok = true;
    // Test prime and composite lengths that are NOT powers of 2
    for (int n : {7, 13, 29, 97, 100, 251, 509}) {
        std::vector<cd> x(n);
        for (auto& v : x)
            v = cd((static_cast<int>(rng() % 200) - 100) / 10.0,
                   (static_cast<int>(rng() % 200) - 100) / 10.0);

        auto X_czt = dft_arbitrary(x, false);
        auto X_ref = naive_dft(x);
        auto x_rec = dft_arbitrary(X_czt, true);

        double err_dft = 0.0, err_idft = 0.0;
        for (int i = 0; i < n; ++i) {
            err_dft  = std::max(err_dft,  std::abs(X_czt[i] - X_ref[i]));
            err_idft = std::max(err_idft, std::abs(x_rec[i] - x[i]));
        }
        if (err_dft > 1e-9 || err_idft > 1e-9) ok = false;
        std::cout << "Length N = " << std::setw(4) << n
                  << " | DFT max err = " << std::scientific << std::setprecision(2) << err_dft
                  << " | IDFT round-trip err = " << err_idft << "\n";
    }

    std::cout << "\nPrime & arbitrary-length Bluestein DFT vs O(N^2) DFT: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
