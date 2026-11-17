/**
 * ============================================================================
 * Fast Walsh-Hadamard Transform (FWHT - XOR, AND, OR Bitwise Convolutions)
 * ============================================================================
 * 
 * Description:
 * The Fast Walsh-Hadamard Transform (FWHT) computes bitwise convolutions
 * between two sequences A and B of length N = 2^k in O(N log N) time:
 *   C[k] = sum_{i (op) j = k} A[i] * B[j]
 * where (op) can be bitwise XOR (^), bitwise AND (&), or bitwise OR (|).
 * 
 * Mathematical Formulation:
 * 1. XOR Convolution:
 *    Transform Matrix H_1 = [ 1  1 ]
 *                           [ 1 -1 ]
 *    Forward: (u, v) -> (u + v, u - v)
 *    Inverse: (u, v) -> ((u + v)/2, (u - v)/2)
 * 
 * 2. AND Convolution:
 *    Transform Matrix = [ 1  1 ]
 *                       [ 0  1 ]
 *    Forward: (u, v) -> (u + v, v)
 *    Inverse: (u, v) -> (u - v, v)
 * 
 * 3. OR Convolution:
 *    Transform Matrix = [ 1  0 ]
 *                       [ 1  1 ]
 *    Forward: (u, v) -> (u, u + v)
 *    Inverse: (u, v) -> (u, v - u)
 * 
 * Convolution Pipeline:
 *   FWHT(C) = FWHT(A) .* FWHT(B)   (pointwise multiplication)
 *   C = IFWHT(FWHT(C))
 * 
 * Time Complexity:
 * - FWHT Transform: O(N log N)
 * - Pointwise Multiplication: O(N)
 * - Inverse FWHT: O(N log N)
 * - Total Runtime: O(N log N) vs O(N^2) brute force
 * - Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>

namespace fwht {

enum OperationType {
    XOR,
    AND,
    OR
};

/**
 * In-place Fast Walsh-Hadamard Transform / Inverse Transform.
 */
void transform(std::vector<int64_t>& a, bool invert, OperationType op) {
    int n = static_cast<int>(a.size());

    for (int len = 1; 2 * len <= n; len <<= 1) {
        for (int i = 0; i < n; i += 2 * len) {
            for (int j = 0; j < len; ++j) {
                int64_t u = a[i + j];
                int64_t v = a[i + len + j];

                if (op == XOR) {
                    if (!invert) {
                        a[i + j] = u + v;
                        a[i + len + j] = u - v;
                    } else {
                        a[i + j] = (u + v) / 2;
                        a[i + len + j] = (u - v) / 2;
                    }
                } else if (op == AND) {
                    if (!invert) {
                        a[i + j] = u + v;
                        a[i + len + j] = v;
                    } else {
                        a[i + j] = u - v;
                        a[i + len + j] = v;
                    }
                } else if (op == OR) {
                    if (!invert) {
                        a[i + j] = u;
                        a[i + len + j] = u + v;
                    } else {
                        a[i + j] = u;
                        a[i + len + j] = v - u;
                    }
                }
            }
        }
    }
}

/**
 * Computes bitwise convolution C[k] = sum_{i (op) j = k} A[i] * B[j] in O(N log N).
 */
std::vector<int64_t> convolve(std::vector<int64_t> a, std::vector<int64_t> b, OperationType op) {
    int n = 1;
    while (n < static_cast<int>(std::max(a.size(), b.size()))) {
        n <<= 1;
    }
    a.resize(n, 0);
    b.resize(n, 0);

    transform(a, false, op);
    transform(b, false, op);

    std::vector<int64_t> c(n);
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] * b[i];
    }

    transform(c, true, op);
    return c;
}

/**
 * Brute force convolution in O(N^2) for validation.
 */
std::vector<int64_t> bruteConvolve(const std::vector<int64_t>& a, const std::vector<int64_t>& b, OperationType op) {
    int n = static_cast<int>(a.size());
    std::vector<int64_t> c(n, 0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int k = 0;
            if (op == XOR) k = i ^ j;
            else if (op == AND) k = i & j;
            else if (op == OR) k = i | j;

            if (k < n) {
                c[k] += a[i] * b[j];
            }
        }
    }
    return c;
}

} // namespace fwht

int main() {
    std::cout << "========================================================\n";
    std::cout << " Fast Walsh-Hadamard Transform (FWHT Convolutions)\n";
    std::cout << "========================================================\n\n";

    // Sample arrays of length 8 = 2^3
    std::vector<int64_t> a = {2, 1, 3, 5, 0, 4, 1, 2};
    std::vector<int64_t> b = {1, 4, 2, 0, 3, 1, 2, 5};
    int n = static_cast<int>(a.size());

    std::cout << "Input Vectors (N = " << n << "):\n";
    std::cout << "  A: [";
    for (int i = 0; i < n; ++i) std::cout << a[i] << (i + 1 < n ? ", " : "]\n");
    std::cout << "  B: [";
    for (int i = 0; i < n; ++i) std::cout << b[i] << (i + 1 < n ? ", " : "]\n\n");

    const char* op_names[] = {"XOR Convolution (^)", "AND Convolution (&)", "OR Convolution (|)"};
    fwht::OperationType op_types[] = {fwht::XOR, fwht::AND, fwht::OR};

    for (int t = 0; t < 3; ++t) {
        std::cout << "--------------------------------------------------------\n";
        std::cout << " " << op_names[t] << ":\n";
        std::cout << "--------------------------------------------------------\n";

        auto fast_res = fwht::convolve(a, b, op_types[t]);
        auto brute_res = fwht::bruteConvolve(a, b, op_types[t]);

        std::cout << std::setw(6) << "Index"
                  << std::setw(15) << "FWHT O(N log N)"
                  << std::setw(15) << "Brute O(N^2)"
                  << std::setw(12) << "Match?\n";
        std::cout << "--------------------------------------------------------\n";

        bool matched = true;
        for (int i = 0; i < n; ++i) {
            std::cout << std::setw(6) << i
                      << std::setw(15) << fast_res[i]
                      << std::setw(15) << brute_res[i]
                      << std::setw(12) << (fast_res[i] == brute_res[i] ? "YES" : "NO")
                      << "\n";
            if (fast_res[i] != brute_res[i]) matched = false;
        }

        std::cout << "Verification: " << (matched ? "PASSED (100% Match)" : "FAILED") << "\n\n";
    }

    return 0;
}
