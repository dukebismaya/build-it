/*
 * ============================================================
 *                 Lychrel Number Candidate Checker
 * ============================================================
 *
 * Description:
 *   A Lychrel number is a natural number that never forms a
 *   palindrome through the iterative process of reversing its
 *   digits and adding the result to the original number. No
 *   Lychrel number has been proven to exist in base 10, but
 *   196 is the most famous candidate (no palindrome found
 *   after millions of iterations).
 *
 *   This program checks whether a given number reaches a
 *   palindrome within a specified number of iterations of the
 *   "reverse and add" process. Numbers that do NOT converge
 *   are flagged as Lychrel candidates.
 *
 * Algorithm:
 *   1. Given number N and max iterations K:
 *   2. Repeat up to K times:
 *      a. rev = reverse_digits(N)
 *      b. N = N + rev
 *      c. If N is a palindrome, return (false, iteration_count)
 *   3. If no palindrome found, return (true, K)
 *
 * Complexity:
 *   Time:  O(K * D) where D is the number of digits
 *   Space: O(D) for string representation
 *
 * Note:
 *   We use arbitrary precision (string-based addition) to
 *   handle the large numbers that arise from iterated addition.
 * ============================================================
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <iomanip>

namespace lychrel_checker {

    // Check if a string represents a palindrome
    bool is_palindrome(const std::string& s) {
        int left = 0;
        int right = static_cast<int>(s.size()) - 1;
        while (left < right) {
            if (s[left] != s[right]) return false;
            ++left;
            --right;
        }
        return true;
    }

    // Reverse a numeric string
    std::string reverse_string(const std::string& s) {
        std::string rev = s;
        std::reverse(rev.begin(), rev.end());
        return rev;
    }

    // Add two non-negative numeric strings (arbitrary precision)
    std::string add_strings(const std::string& a, const std::string& b) {
        std::string result;
        int carry = 0;
        int i = static_cast<int>(a.size()) - 1;
        int j = static_cast<int>(b.size()) - 1;

        while (i >= 0 || j >= 0 || carry) {
            int sum = carry;
            if (i >= 0) sum += a[i--] - '0';
            if (j >= 0) sum += b[j--] - '0';
            result.push_back('0' + sum % 10);
            carry = sum / 10;
        }

        std::reverse(result.begin(), result.end());
        return result;
    }

    // Check if a number is a Lychrel candidate within max_iter iterations
    // Returns {is_candidate, iterations_used, final_value}
    struct LychrelResult {
        bool is_candidate;
        int iterations;
        std::string final_value;
    };

    LychrelResult check_lychrel(const std::string& number, int max_iter) {
        std::string current = number;

        for (int iter = 1; iter <= max_iter; ++iter) {
            std::string rev = reverse_string(current);
            current = add_strings(current, rev);

            if (is_palindrome(current)) {
                return {false, iter, current};
            }
        }

        return {true, max_iter, current};
    }

} // namespace lychrel_checker

int main() {
    using namespace lychrel_checker;

    std::cout << "============================================================\n";
    std::cout << "           Lychrel Number Candidate Checker\n";
    std::cout << "============================================================\n\n";

    // Test well-known non-Lychrel numbers that converge quickly
    struct TestCase {
        std::string number;
        int max_iter;
    };

    std::vector<TestCase> tests = {
        {"56",    50},   // 56 + 65 = 121 (palindrome in 1 step)
        {"57",    50},   // Takes a few steps
        {"59",    50},   // 59 + 95 = 154 + 451 = 605 + 506 = 1111 (3 steps)
        {"89",   100},   // Famous: takes 24 steps to reach palindrome
        {"10911", 100},  // Takes many steps
        {"196",  500},   // Famous Lychrel candidate
        {"879",  500},   // Another Lychrel candidate
        {"1997", 500},   // Another Lychrel candidate
    };

    std::cout << std::left << std::setw(10) << "Number"
              << std::setw(14) << "Result"
              << std::setw(12) << "Iterations"
              << "Final Value (truncated)\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto& test : tests) {
        auto result = check_lychrel(test.number, test.max_iter);

        std::string status = result.is_candidate ? "CANDIDATE" : "Converged";
        std::string display_val = result.final_value;
        if (display_val.size() > 30) {
            display_val = display_val.substr(0, 27) + "...";
        }

        std::cout << std::setw(10) << test.number
                  << std::setw(14) << status
                  << std::setw(12) << result.iterations
                  << display_val << "\n";
    }

    // Detailed trace for number 89 (famous for taking 24 steps)
    std::cout << "\n--- Detailed trace for 89 ---\n";
    std::string current = "89";
    for (int step = 1; step <= 30; ++step) {
        std::string rev = reverse_string(current);
        std::string next = add_strings(current, rev);
        std::cout << "Step " << std::setw(2) << step << ": "
                  << current << " + " << rev << " = " << next;

        if (is_palindrome(next)) {
            std::cout << "  [PALINDROME]\n";
            break;
        }
        std::cout << "\n";
        current = next;
    }

    // Scan range 1..200 for Lychrel candidates with 50-iteration limit
    std::cout << "\n--- Lychrel candidates in [1, 200] (50-iteration limit) ---\n";
    std::vector<int> candidates;
    for (int n = 1; n <= 200; ++n) {
        auto result = check_lychrel(std::to_string(n), 50);
        if (result.is_candidate) {
            candidates.push_back(n);
        }
    }

    std::cout << "Found " << candidates.size() << " candidates: ";
    for (size_t i = 0; i < candidates.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << candidates[i];
    }
    std::cout << "\n";

    std::cout << "\n============================================================\n";
    std::cout << "  Note: No base-10 Lychrel number has been proven to exist.\n";
    std::cout << "  196 has been tested to billions of iterations without\n";
    std::cout << "  forming a palindrome.\n";
    std::cout << "============================================================\n";

    return 0;
}
