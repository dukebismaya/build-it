/**
 * @file leonardo_number_generator.cpp
 * @brief Leonardo number generator and Smoothsort connection in C++17.
 *
 * Mathematical Definition:
 *   The Leonardo numbers are defined by the recurrence:
 *     L(0) = 1
 *     L(1) = 1
 *     L(n) = L(n - 1) + L(n - 2) + 1,  for n >= 2.
 *
 * The sequence begins:
 *   1, 1, 3, 5, 9, 15, 25, 41, 67, 109, 177, 287, 465, 753, 1219, 1973, ...
 *
 * Fundamental Identity with Fibonacci Numbers:
 *   L(n) = 2 * F(n + 1) - 1
 *   where F(n) is the standard Fibonacci sequence: F(0)=0, F(1)=1, F(2)=1, F(3)=2, ...
 *
 * Algorithmic Application (Smoothsort):
 *   Leonardo numbers represent the sizes of full Leonardo trees used in
 *   Edsger W. Dijkstra's Smoothsort algorithm (an adaptive in-place sorting algorithm
 *   running in O(N) on nearly-sorted inputs and O(N log N) worst-case).
 *
 * Complexity:
 *   - Time: O(N) to generate the first N terms.
 *   - Space: O(N) storage (or O(1) iterative state).
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdint>

namespace sequences {

/**
 * @brief Generates standard Fibonacci numbers F(0) to F(n).
 */
std::vector<uint64_t> generateFibonacci(size_t n) {
    if (n == 0) return {0};
    std::vector<uint64_t> f(n + 1, 0);
    f[0] = 0;
    f[1] = 1;
    for (size_t i = 2; i <= n; ++i) {
        f[i] = f[i - 1] + f[i - 2];
    }
    return f;
}

/**
 * @brief Generates Leonardo numbers L(0) to L(n).
 */
std::vector<uint64_t> generateLeonardo(size_t n) {
    if (n == 0) return {1};
    if (n == 1) return {1, 1};

    std::vector<uint64_t> l(n + 1, 1);
    for (size_t i = 2; i <= n; ++i) {
        l[i] = l[i - 1] + l[i - 2] + 1;
    }
    return l;
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "      LEONARDO NUMBER GENERATOR         \n";
    std::cout << "  (Smoothsort & Fibonacci Connection)   \n";
    std::cout << "========================================\n\n";

    size_t count = 20;
    auto leonardo = sequences::generateLeonardo(count);
    auto fibonacci = sequences::generateFibonacci(count + 2);

    std::cout << std::left << std::setw(6)  << "n"
              << std::setw(16) << "Leonardo L(n)"
              << std::setw(18) << "2*F(n+1) - 1"
              << "Identity Check\n";
    std::cout << std::string(54, '-') << "\n";

    for (size_t n = 0; n <= count; ++n) {
        uint64_t fibForm = 2ULL * fibonacci[n + 1] - 1ULL;
        bool match = (leonardo[n] == fibForm);

        std::cout << std::left << std::setw(6)  << n
                  << std::setw(16) << leonardo[n]
                  << std::setw(18) << fibForm
                  << (match ? "EXACT MATCH" : "FAIL") << "\n";
    }

    std::cout << "\nSmoothsort Leonardo Tree Decomposition:\n";
    std::cout << "A Leonardo tree of order k has root with left child of order (k - 1)\n";
    std::cout << "and right child of order (k - 2), giving total size: L(k) = L(k-1) + L(k-2) + 1.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
