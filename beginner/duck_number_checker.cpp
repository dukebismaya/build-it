/*
 * Program Description:
 * Duck Number Checker in C++.
 * A Duck Number is a positive number which has zeroes present in it,
 * but there should be NO zero present in the beginning of the number (no leading zeroes).
 *
 * Examples:
 * - 3210 : Duck Number (contains 0, not at start)
 * - 7056 : Duck Number (contains 0, not at start)
 * - 8421 : NOT a Duck Number (no 0 present)
 * - 0123 : NOT a Duck Number (leading zero)
 *
 * Time Complexity: O(log10(N)) / O(String Length)
 * Space Complexity: O(1).
 */

#include <iostream>
#include <string>
#include <vector>

bool isDuckNumber(const std::string& str) {
    if (str.empty()) return false;

    // Ignore leading whitespace if any
    size_t start = 0;
    while (start < str.length() && str[start] == ' ') start++;

    // If starts with '0', it is not a duck number
    if (start < str.length() && str[start] == '0') {
        return false;
    }

    // Check if there is any zero following the first non-zero digit
    bool hasZero = false;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '0') {
            hasZero = true;
            break;
        }
    }

    return hasZero;
}

bool isDuckNumberInt(long long num) {
    if (num <= 0) return false;
    return isDuckNumber(std::to_string(num));
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "           Duck Number Checker            \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> testCases = {
        "1023",
        "7056",
        "3210",
        "8421",
        "0123",
        "5000",
        "9"
    };

    for (const auto& s : testCases) {
        bool isDuck = isDuckNumber(s);
        std::cout << "Number: " << s
                  << " -> " << (isDuck ? "DUCK NUMBER [YES]" : "NOT a Duck Number [NO]") << "\n";
    }

    std::cout << "\nFirst 10 Duck Numbers:\n";
    int count = 0;
    for (long long i = 1; count < 10; ++i) {
        if (isDuckNumberInt(i)) {
            std::cout << i << " ";
            count++;
        }
    }
    std::cout << "\n";

    return 0;
}
