/*
 * Problem Description:
 * Knuth-Morris-Pratt (KMP) Pattern Matching Algorithm with failure function construction.
 * Given a text string T and a pattern P, find all occurrences of P in T.
 * Unlike the naive approach (O(N*M)), KMP pre-computes a Longest Proper Prefix which is also
 * Suffix (LPS) array for the pattern, allowing it to skip previously matched characters.
 * Time Complexity: O(N + M) where N = length of text, M = length of pattern.
 * Space Complexity: O(M) for the LPS array.
 */

#include <iostream>
#include <vector>
#include <string>

// Build the Longest Proper Prefix which is also Suffix (LPS) array
std::vector<int> buildLPS(const std::string& pattern) {
    int m = pattern.size();
    std::vector<int> lps(m, 0);
    int len = 0;
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

// KMP search: returns starting indices of all matches
std::vector<int> kmpSearch(const std::string& text, const std::string& pattern) {
    int n = text.size();
    int m = pattern.size();
    std::vector<int> matches;

    if (m == 0 || n < m) return matches;

    std::vector<int> lps = buildLPS(pattern);

    int i = 0; // index into text
    int j = 0; // index into pattern

    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }
        if (j == m) {
            matches.push_back(i - j);
            j = lps[j - 1];
        } else if (i < n && text[i] != pattern[j]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return matches;
}

int main() {
    std::string text = "ABABDABACDABABCABAB";
    std::string pattern = "ABABCABAB";

    std::cout << "KMP Pattern Matching Algorithm\n";
    std::cout << "Text:    \"" << text << "\"\n";
    std::cout << "Pattern: \"" << pattern << "\"\n\n";

    // Show LPS array
    std::vector<int> lps = buildLPS(pattern);
    std::cout << "LPS Array: [";
    for (size_t i = 0; i < lps.size(); ++i) {
        std::cout << lps[i];
        if (i < lps.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n\n";

    std::vector<int> matches = kmpSearch(text, pattern);

    if (matches.empty()) {
        std::cout << "Pattern not found in text.\n";
    } else {
        std::cout << "Pattern found at index(es): ";
        for (int idx : matches) {
            std::cout << idx << " ";
        }
        std::cout << "\n";
    }

    // Additional test
    std::string text2 = "AAAAABAAABA";
    std::string pattern2 = "AAAB";
    auto matches2 = kmpSearch(text2, pattern2);
    std::cout << "\nText:    \"" << text2 << "\"\n";
    std::cout << "Pattern: \"" << pattern2 << "\"\n";
    std::cout << "Found at: ";
    for (int idx : matches2) std::cout << idx << " ";
    std::cout << "\n";

    return 0;
}
