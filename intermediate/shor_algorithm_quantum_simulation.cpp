/*
 * Problem Description:
 * Conceptualize Shor's Algorithm for integer factorization.
 * Shor's algorithm is a quantum computer algorithm for finding the prime factors of an integer.
 */

#include <iostream>
#include <numeric>
#include <cmath>
using namespace std;

int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

void shorsAlgorithmConcept(int N) {
    cout << "Shor's Algorithm Conceptual Flow for N = " << N << "\n";
    cout << "1. Classical: Choose a random number 'a' < N.\n";
    int a = 2; // For demonstration
    cout << "   Chosen a = " << a << "\n";
    
    int g = gcd(a, N);
    if (g > 1) {
        cout << "2. Classical: Found factor via GCD! Factor = " << g << "\n";
        return;
    }
    
    cout << "3. Quantum Subroutine: Find the period 'r' of the function f(x) = a^x mod N.\n";
    cout << "   (This is where Quantum Fourier Transform is used).\n";
    
    // Simulate finding period r classically (extremely slow for large N)
    int r = 1;
    long long val = a % N;
    while (val != 1) {
        val = (val * a) % N;
        r++;
    }
    cout << "   Found period r = " << r << "\n";
    
    if (r % 2 != 0) {
        cout << "4. Classical: r is odd, algorithm fails for this 'a'. Retry.\n";
        return;
    }
    
    long long x = pow(a, r / 2);
    x = x % N;
    
    if (x == N - 1) {
        cout << "4. Classical: x is trivial, algorithm fails. Retry.\n";
        return;
    }
    
    int factor1 = gcd(x + 1, N);
    int factor2 = gcd(x - 1, N);
    
    cout << "5. Classical: Factors found: " << factor1 << " and " << factor2 << "\n";
}

int main() {
    shorsAlgorithmConcept(15);
    return 0;
}
