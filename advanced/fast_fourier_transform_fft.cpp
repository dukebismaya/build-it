/**
 * @file fast_fourier_transform_fft.cpp
 * @brief Computes Fast Fourier Transform (FFT) and performs O(N log N) polynomial convolution.
 *
 * Theory:
 * The Discrete Fourier Transform (DFT) evaluates a polynomial of degree N-1 at the N complex
 * roots of unity omega_N^k = exp(2 * pi * i * k / N).
 *
 * The Cooley-Tukey algorithm computes DFT and Inverse DFT (IDFT) in O(N log N) time
 * by recursively splitting coefficients into even and odd indices (radix-2 DIT).
 *
 * Polynomial Multiplication via Convolution Theorem:
 *   A(x) * B(x) = IDFT( DFT(A) . DFT(B) )
 * where '.' denotes element-wise Hadamard product.
 *
 * Time Complexity: O(N log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <algorithm>
#include <iomanip>

using Complex = std::complex<double>;
const double PI = std::acos(-1.0);

/**
 * @brief In-place iterative Radix-2 Cooley-Tukey Fast Fourier Transform.
 * @param a Vector of complex coefficients (size must be a power of 2).
 * @param invert If true, computes Inverse FFT (IDFT).
 */
void fft(std::vector<Complex>& a, bool invert) {
    int n = static_cast<int>(a.size());

    // 1. Bit-reversal permutation
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) {
            j ^= bit;
        }
        j ^= bit;

        if (i < j) {
            std::swap(a[i], a[j]);
        }
    }

    // 2. Butterfly operations
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        Complex wlen(std::cos(ang), std::sin(ang));

        for (int i = 0; i < n; i += len) {
            Complex w(1.0, 0.0);
            for (int j = 0; j < len / 2; ++j) {
                Complex u = a[i + j];
                Complex v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (Complex& x : a) {
            x /= n;
        }
    }
}

/**
 * @brief Multiplies two integer polynomials in O(N log N) time using FFT.
 * @param a Coefficients of first polynomial.
 * @param b Coefficients of second polynomial.
 * @return Coefficients of resulting product polynomial.
 */
std::vector<long long> multiplyPolynomialsFFT(const std::vector<long long>& a,
                                             const std::vector<long long>& b) {
    if (a.empty() || b.empty()) return {};

    std::vector<Complex> fa(a.begin(), a.end());
    std::vector<Complex> fb(b.begin(), b.end());

    int n = 1;
    size_t targetSize = a.size() + b.size() - 1;
    while (n < static_cast<int>(targetSize)) {
        n <<= 1;
    }

    fa.resize(n);
    fb.resize(n);

    // Forward FFT
    fft(fa, false);
    fft(fb, false);

    // Pointwise multiplication
    for (int i = 0; i < n; ++i) {
        fa[i] *= fb[i];
    }

    // Inverse FFT
    fft(fa, true);

    std::vector<long long> result(targetSize);
    for (size_t i = 0; i < targetSize; ++i) {
        result[i] = static_cast<long long>(std::round(fa[i].real()));
    }

    return result;
}

int main() {
    std::cout << "=== Fast Fourier Transform (FFT) Polynomial Multiplication ===\n\n";

    // Example 1:
    // A(x) = 1 + 2x + 3x^2
    // B(x) = 4 + 5x + 6x^2
    // Product C(x) = 4 + 13x + 28x^2 + 27x^3 + 18x^4
    std::vector<long long> A = {1, 2, 3};
    std::vector<long long> B = {4, 5, 6};

    std::cout << "1. Small Test Case:\n";
    std::cout << "   A(x) coeffs: [ ";
    for (long long c : A) std::cout << c << " ";
    std::cout << "]\n   B(x) coeffs: [ ";
    for (long long c : B) std::cout << c << " ";
    std::cout << "]\n";

    auto C = multiplyPolynomialsFFT(A, B);

    std::cout << "   Product C(x) via FFT: [ ";
    for (long long c : C) std::cout << c << " ";
    std::cout << "]\n";
    std::cout << "   Expected:             [ 4 13 28 27 18 ]\n\n";

    // Example 2: Higher degree multiplication
    std::vector<long long> poly1 = {1, -2, 3, -4, 5};
    std::vector<long long> poly2 = {2, 0, -1, 4};

    std::cout << "2. Signed Coefficients Test:\n";
    std::cout << "   Poly1: [ ";
    for (long long c : poly1) std::cout << c << " ";
    std::cout << "]\n   Poly2: [ ";
    for (long long c : poly2) std::cout << c << " ";
    std::cout << "]\n";

    auto prod2 = multiplyPolynomialsFFT(poly1, poly2);

    std::cout << "   Product via FFT: [ ";
    for (long long c : prod2) std::cout << c << " ";
    std::cout << "]\n\n";

    // Example 3: Large degree test
    const int DEG = 64;
    std::vector<long long> large1(DEG, 1); // 1 + x + x^2 + ... + x^(DEG-1)
    std::vector<long long> large2(DEG, 1);

    auto largeProd = multiplyPolynomialsFFT(large1, large2);
    std::cout << "3. Degree " << (DEG - 1) << " All-Ones Polynomial Convolution:\n";
    std::cout << "   Product Degree: " << (largeProd.size() - 1) << " terms.\n";
    std::cout << "   First 5 terms:  [ ";
    for (int i = 0; i < 5; ++i) std::cout << largeProd[i] << " ";
    std::cout << "]\n";
    std::cout << "   Peak center term at index " << (DEG - 1) << ": " << largeProd[DEG - 1] 
              << " (Expected: " << DEG << ")\n";

    return 0;
}

