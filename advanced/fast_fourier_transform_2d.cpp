/**
 * @file fast_fourier_transform_2d.cpp
 * @brief Computes 2D Fast Fourier Transform (2D FFT) and 2D Spatial Convolution in O(M * N * log(M * N)).
 *
 * Theory:
 * The 2D Discrete Fourier Transform of an M x N matrix A is defined as:
 *   F(u, v) = sum_{x=0}^{M-1} sum_{y=0}^{N-1} A(x, y) * exp(-2*pi*i * (u*x/M + v*y/N))
 *
 * Separability Property:
 * The 2D DFT kernel is separable: exp(-2*pi*i*(ux/M + vy/N)) = exp(-2*pi*i*ux/M) * exp(-2*pi*i*vy/N).
 * Therefore, 2D FFT can be computed by:
 *   1. Applying 1D FFT across each of the M rows.
 *   2. Applying 1D FFT down each of the N columns.
 *
 * 2D Convolution Theorem:
 *   A * B = IFFT2D( FFT2D(A) . FFT2D(B) )
 * Allows 2D image filtering, blurring, and pattern matching in O(M*N log(MN)).
 *
 * Time Complexity: O(M * N * log(M * N))
 * Space Complexity: O(M * N)
 */

#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <iomanip>

using Complex = std::complex<double>;
const double PI = std::acos(-1.0);

class FFT2D {
private:
    static void fft1D(std::vector<Complex>& a, bool invert) {
        int n = static_cast<int>(a.size());

        for (int i = 1, j = 0; i < n; ++i) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) {
                j ^= bit;
            }
            j ^= bit;
            if (i < j) std::swap(a[i], a[j]);
        }

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

public:
    /**
     * @brief Computes 2D FFT or 2D IFFT of an M x N complex matrix (M, N must be powers of 2).
     */
    static void transform2D(std::vector<std::vector<Complex>>& matrix, bool invert) {
        int m = static_cast<int>(matrix.size());
        int n = static_cast<int>(matrix[0].size());

        // 1. Transform each row
        for (int i = 0; i < m; ++i) {
            fft1D(matrix[i], invert);
        }

        // 2. Transform each column
        std::vector<Complex> col(m);
        for (int j = 0; j < n; ++j) {
            for (int i = 0; i < m; ++i) {
                col[i] = matrix[i][j];
            }
            fft1D(col, invert);
            for (int i = 0; i < m; ++i) {
                matrix[i][j] = col[i];
            }
        }
    }

    /**
     * @brief Performs 2D convolution of two real matrices.
     */
    static std::vector<std::vector<double>> convolve2D(
        const std::vector<std::vector<double>>& A,
        const std::vector<std::vector<double>>& B) {
        int rA = static_cast<int>(A.size());
        int cA = static_cast<int>(A[0].size());
        int rB = static_cast<int>(B.size());
        int cB = static_cast<int>(B[0].size());

        int outR = rA + rB - 1;
        int outC = cA + cB - 1;

        int m = 1, n = 1;
        while (m < outR) m <<= 1;
        while (n < outC) n <<= 1;

        std::vector<std::vector<Complex>> fa(m, std::vector<Complex>(n, 0.0));
        std::vector<std::vector<Complex>> fb(m, std::vector<Complex>(n, 0.0));

        for (int i = 0; i < rA; ++i) {
            for (int j = 0; j < cA; ++j) {
                fa[i][j] = A[i][j];
            }
        }

        for (int i = 0; i < rB; ++i) {
            for (int j = 0; j < cB; ++j) {
                fb[i][j] = B[i][j];
            }
        }

        transform2D(fa, false);
        transform2D(fb, false);

        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                fa[i][j] *= fb[i][j];
            }
        }

        transform2D(fa, true);

        std::vector<std::vector<double>> result(outR, std::vector<double>(outC));
        for (int i = 0; i < outR; ++i) {
            for (int j = 0; j < outC; ++j) {
                result[i][j] = fa[i][j].real();
            }
        }

        return result;
    }
};

void printRealMatrix(const std::vector<std::vector<double>>& mat) {
    for (const auto& row : mat) {
        std::cout << "  ";
        for (double val : row) {
            if (std::abs(val) < 1e-9) val = 0.0;
            std::cout << std::fixed << std::setprecision(2) << std::setw(7) << val;
        }
        std::cout << '\n';
    }
}

int main() {
    std::cout << "=== 2D Fast Fourier Transform & Spatial Convolution ===\n\n";

    // 3x3 Image Patch
    std::vector<std::vector<double>> image = {
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0}
    };

    // 2x2 Filter Kernel (e.g., edge / gradient detector)
    std::vector<std::vector<double>> kernel = {
        { 1.0, -1.0},
        {-1.0,  1.0}
    };

    std::cout << "Original Matrix A (3x3):\n";
    printRealMatrix(image);

    std::cout << "\nConvolution Kernel B (2x2):\n";
    printRealMatrix(kernel);

    auto result = FFT2D::convolve2D(image, kernel);

    std::cout << "\n2D Convolution Result (A * B) of Size 4x4:\n";
    printRealMatrix(result);

    return 0;
}

