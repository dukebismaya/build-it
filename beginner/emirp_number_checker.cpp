/**
 * @file emirp_number_checker.cpp
 * @brief Checks if a number is an Emirp number and generates emirps in a range.
 *
 * Definition:
 * An Emirp (prime spelled backwards) is a prime number that results in a DIFFERENT
 * prime when its decimal digits are reversed.
 *
 * Requirements:
 * 1. N is prime.
 * 2. reverse(N) is prime.
 * 3. N != reverse(N) (palindromic primes like 11, 101, 131 are excluded).
 *
 * Examples of Emirps:
 * - 13 <-> 31 (both prime, distinct)
 * - 17 <-> 71 (both prime, distinct)
 * - 37 <-> 73 (both prime, distinct)
 * - 79 <-> 97 (both prime, distinct)
 * - 107 <-> 701 (both prime, distinct)
 * - 113 <-> 311 (both prime, distinct)
 *
 * Time Complexity:
 * - Single check: O(sqrt(N)) primality tests
 * - Range search up to limit L: O(L * sqrt(L)) or O(L log(log L)) via sieve
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

/**
 * @brief Checks whether an integer n is prime.
 */
bool isPrime(long long n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

/**
 * @brief Reverses the decimal digits of a non-negative integer.
 */
long long reverseDigits(long long n) {
    long long rev = 0;
    while (n > 0) {
        rev = rev * 10 + (n % 10);
        n /= 10;
    }
    return rev;
}

/**
 * @brief Checks if a number is an Emirp number.
 */
bool isEmirp(long long n) {
    if (!isPrime(n)) return false;

    long long rev = reverseDigits(n);
    if (rev == n) return false; // Palindromic primes are not emirps

    return isPrime(rev);
}

/**
 * @brief Generates all Emirp numbers up to a specified limit.
 */
std::vector<std::pair<long long, long long>> findEmirps(long long limit) {
    std::vector<std::pair<long long, long long>> results;
    for (long long i = 10; i <= limit; ++i) {
        if (isEmirp(i)) {
            results.push_back({i, reverseDigits(i)});
        }
    }
    return results;
}

int main() {
    std::cout << "=== Emirp (Reversible Prime) Number Checker & Generator ===\n\n";

    // 1. Check selected numbers
    std::vector<long long> testCandidates = {11, 13, 17, 19, 23, 37, 79, 101, 107, 113, 131, 149};

    std::cout << "1. Verification of Selected Numbers:\n";
    std::cout << "  " << std::setw(8) << "Number"
              << std::setw(12) << "Is Prime?"
              << std::setw(12) << "Reversed"
              << std::setw(16) << "Rev is Prime?"
              << std::setw(14) << "Is Emirp?" << '\n';
    std::cout << "  " << std::string(62, '-') << '\n';

    for (long long n : testCandidates) {
        bool p1 = isPrime(n);
        long long rev = reverseDigits(n);
        bool p2 = isPrime(rev);
        bool emirp = isEmirp(n);

        std::cout << "  " << std::setw(8) << n
                  << std::setw(12) << (p1 ? "YES" : "No")
                  << std::setw(12) << rev
                  << std::setw(16) << (p2 ? "YES" : "No")
                  << std::setw(14) << (emirp ? "YES" : "No") << '\n';
    }

    // 2. Range search up to 500
    std::cout << "\n2. All Emirp Numbers up to 500:\n";
    auto emirps = findEmirps(500);
    int col = 0;
    std::cout << "  ";
    for (const auto& [p, revP] : emirps) {
        std::cout << p << " (<->" << revP << ")\t";
        col++;
        if (col % 4 == 0) {
            std::cout << "\n  ";
        }
    }
    std::cout << "\n\nTotal Emirps found in [10, 500]: " << emirps.size() << '\n';

    return 0;
}

