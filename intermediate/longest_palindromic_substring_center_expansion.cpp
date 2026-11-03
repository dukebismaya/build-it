/**
 * @file longest_palindromic_substring_center_expansion.cpp
 * @brief Finds the Longest Palindromic Substring using the Expand-Around-Center technique.
 *
 * Algorithm:
 * - A palindrome reads the same forwards and backwards and is symmetric around its center.
 * - In a string of length N, there are 2N - 1 possible centers:
 *   - N centers on single characters (odd-length palindromes: center at i)
 *   - N - 1 centers between adjacent characters (even-length palindromes: center between i and i+1)
 * - For each center, expand outward as long as the left and right characters match.
 * - Track the maximum length and starting index.
 *
 * Advantages:
 * - Unlike the 2D Dynamic Programming approach which requires O(N^2) memory,
 *   this algorithm uses strictly O(1) auxiliary space!
 *
 * Time Complexity: O(N^2) worst case, O(N) average case for typical strings
 * Space Complexity: O(1) auxiliary space
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

struct PalindromeSubstringResult {
    std::string palindrome;
    int startIndex;
    int length;
};

/**
 * @brief Expands around a center (left, right) and returns the length of the palindrome.
 */
int expandAroundCenter(const std::string& s, int left, int right) {
    int n = static_cast<int>(s.length());
    while (left >= 0 && right < n && s[left] == s[right]) {
        --left;
        ++right;
    }
    // Length is (right - 1) - (left + 1) + 1 = right - left - 1
    return right - left - 1;
}

/**
 * @brief Finds the longest palindromic substring in s.
 */
PalindromeSubstringResult findLongestPalindromicSubstring(const std::string& s) {
    int n = static_cast<int>(s.length());
    if (n <= 1) {
        return {s, 0, n};
    }

    int start = 0;
    int maxLen = 1;

    for (int i = 0; i < n; ++i) {
        // Odd length palindrome centered at i
        int len1 = expandAroundCenter(s, i, i);
        // Even length palindrome centered between i and i + 1
        int len2 = expandAroundCenter(s, i, i + 1);

        int curMax = std::max(len1, len2);
        if (curMax > maxLen) {
            maxLen = curMax;
            start = i - (curMax - 1) / 2;
        }
    }

    return {s.substr(start, maxLen), start, maxLen};
}

int main() {
    std::cout << "=== Longest Palindromic Substring (Expand Around Center) ===\n\n";

    std::vector<std::string> testStrings = {
        "babad",
        "cbbd",
        "racecar",
        "forgeeksskeegfor",
        "abacdfgdcaba",
        "a",
        "ac"
    };

    std::cout << "  " << std::setw(20) << "Input String"
              << std::setw(16) << "Longest Palindrome"
              << std::setw(12) << "Start Idx"
              << std::setw(10) << "Length" << '\n';
    std::cout << "  " << std::string(58, '-') << '\n';

    for (const auto& str : testStrings) {
        auto res = findLongestPalindromicSubstring(str);
        std::cout << "  " << std::setw(20) << ("\"" + str + "\"")
                  << std::setw(16) << ("\"" + res.palindrome + "\"")
                  << std::setw(12) << res.startIndex
                  << std::setw(10) << res.length << '\n';
    }

    return 0;
}

