/*
 * Program Description:
 * Manacher's Algorithm for Longest Palindromic Substring.
 * Finds the longest palindromic substring in strictly linear time O(N).
 *
 * Algorithm Concept:
 * 1. String Preprocessing: Insert delimiter '#' between every character (e.g. "aba" -> "^#a#b#a#$")
 *    This transforms all palindromes (even and odd length) into odd length, removing edge cases.
 * 2. Palindrome Radii Array P[i]:
 *    Maintains the center C and right boundary R of the rightmost palindrome discovered so far.
 *    For position i, mirrors i across C to i_mirror = 2*C - i.
 *    If i < R, P[i] can be initialized to min(R - i, P[i_mirror]).
 * 3. Expands around i only beyond R, updating C and R as needed.
 *
 * Time Complexity: O(N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Transforms string by inserting '#' and sentinel bounds '^' and '$'
std::string preprocessString(const std::string& s) {
    if (s.empty()) return "^$";
    std::string res = "^";
    for (char c : s) {
        res += "#";
        res += c;
    }
    res += "#$";
    return res;
}

// Finds the longest palindromic substring using Manacher's Algorithm
std::string longestPalindromeManacher(const std::string& s) {
    if (s.empty()) return "";

    std::string t = preprocessString(s);
    int n = t.length();
    std::vector<int> p(n, 0); // P[i] is radius of palindrome centered at i
    int c = 0, r = 0;

    for (int i = 1; i < n - 1; ++i) {
        int iMirror = 2 * c - i;

        if (r > i) {
            p[i] = std::min(r - i, p[iMirror]);
        } else {
            p[i] = 0;
        }

        // Attempt to expand palindrome centered at i
        while (t[i + 1 + p[i]] == t[i - 1 - p[i]]) {
            p[i]++;
        }

        // If palindrome centered at i expands past r, adjust center and right bound
        if (i + p[i] > r) {
            c = i;
            r = i + p[i];
        }
    }

    // Find maximum element in P
    int maxLen = 0;
    int centerIndex = 0;
    for (int i = 1; i < n - 1; ++i) {
        if (p[i] > maxLen) {
            maxLen = p[i];
            centerIndex = i;
        }
    }

    // Map back to original string coordinates
    int start = (centerIndex - 1 - maxLen) / 2;
    return s.substr(start, maxLen);
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Manacher's Algorithm Demo (O(N))    \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> testCases = {
        "babad",
        "cbbd",
        "racecar",
        "forgeeksskeegfor",
        "abacdfgdcaba",
        "civilwartestingwhetherthatnationsoconceivedandatonafterlongago"
    };

    for (const auto& str : testCases) {
        std::string lps = longestPalindromeManacher(str);
        std::cout << "Input String    : \"" << str << "\" (len " << str.length() << ")\n";
        std::cout << "Longest Palindrome: \"" << lps << "\" (len " << lps.length() << ")\n";
        std::cout << "--------------------------------------------------------\n";
    }

    return 0;
}
