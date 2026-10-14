/*
 * Problem Description:
 * Euler's Totient (Phi) Function.
 * For a positive integer n, Euler's Totient function φ(n) counts the number of integers
 * from 1 to n that are coprime (relatively prime) with n, i.e., gcd(k, n) = 1.
 * Properties:
 * - φ(1) = 1
 * - If p is prime: φ(p) = p - 1
 * - If p is prime: φ(p^k) = p^k - p^(k-1)
 * - φ is multiplicative: φ(a*b) = φ(a)*φ(b) when gcd(a,b) = 1
 * - Formula: φ(n) = n * product(1 - 1/p) for all prime factors p of n
 * This implementation also includes a sieve-based approach to compute φ for all numbers up to N.
 * Time Complexity: O(sqrt(n)) for single computation, O(N log log N) for sieve.
 */

#include <iostream>
#include <vector>

// Compute φ(n) for a single number using prime factorization
int eulerTotient(int n) {
    int result = n;
    for (int p = 2; p * p <= n; ++p) {
        if (n % p == 0) {
            while (n % p == 0) {
                n /= p;
            }
            result -= result / p;
        }
    }
    if (n > 1) {
        result -= result / n;
    }
    return result;
}

// Sieve of Euler Totient: compute φ(i) for all i in [0, maxN]
std::vector<int> eulerTotientSieve(int maxN) {
    std::vector<int> phi(maxN + 1);
    for (int i = 0; i <= maxN; ++i) phi[i] = i;

    for (int p = 2; p <= maxN; ++p) {
        if (phi[p] == p) { // p is prime
            for (int k = p; k <= maxN; k += p) {
                phi[k] -= phi[k] / p;
            }
        }
    }
    return phi;
}

int main() {
    std::cout << "Euler's Totient Function φ(n)\n\n";

    // Single computations
    int testValues[] = {1, 6, 10, 12, 36, 97, 100};
    for (int n : testValues) {
        std::cout << "φ(" << n << ") = " << eulerTotient(n) << "\n";
    }

    // Sieve-based computation
    int maxN = 30;
    auto phi = eulerTotientSieve(maxN);
    std::cout << "\nTotient sieve for 1 to " << maxN << ":\n";
    for (int i = 1; i <= maxN; ++i) {
        std::cout << "φ(" << i << ")=" << phi[i];
        if (i < maxN) std::cout << ", ";
        if (i % 10 == 0) std::cout << "\n";
    }
    std::cout << "\n";

    return 0;
}
