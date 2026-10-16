/*
 * Program Description:
 * Z-Algorithm for Linear Time Exact Pattern Matching.
 * Given a text T and pattern P, finds all occurrences of P in T in O(|T| + |P|) time.
 *
 * Z-Array Definition:
 * For a string S of length N, Z[i] is the length of the longest substring starting from S[i]
 * that is also a prefix of S (i.e. longest common prefix of S and S[i..N-1]).
 *
 * Algorithm Strategy:
 * 1. Form concatenated string S = Pattern + '$' + Text (where '$' is not in alphabet).
 * 2. Compute Z-array for S in O(|S|) time maintaining a match window [L, R].
 * 3. Any index i in the Text section where Z[i] == |Pattern| is a match at position (i - |Pattern| - 1).
 *
 * Time Complexity: O(|Text| + |Pattern|)
 * Space Complexity: O(|Text| + |Pattern|)
 */

#include <iostream>
#include <vector>
#include <string>

// Computes the Z-array for string s in O(N) time
std::vector<int> calculateZArray(const std::string& s) {
    int n = s.length();
    std::vector<int> z(n, 0);
    int l = 0, r = 0;

    for (int i = 1; i < n; ++i) {
        if (i <= r) {
            // Inside the current Z-box [l, r]
            z[i] = std::min(r - i + 1, z[i - l]);
        }

        // Try to extend the match beyond r
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }

        // Update the Z-box if we extended past r
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }

    return z;
}

// Finds all starting indices of pattern in text using Z-algorithm
std::vector<int> searchPatternZ(const std::string& text, const std::string& pattern) {
    std::vector<int> matchIndices;
    if (pattern.empty() || text.length() < pattern.length()) {
        return matchIndices;
    }

    // Concatenate with delimiter not present in text/pattern
    std::string concat = pattern + "$" + text;
    std::vector<int> z = calculateZArray(concat);

    int m = pattern.length();
    int totalLen = concat.length();

    for (int i = m + 1; i < totalLen; ++i) {
        if (z[i] == m) {
            // Match found in text: offset by (m + 1)
            matchIndices.push_back(i - m - 1);
        }
    }

    return matchIndices;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "       Z-Algorithm Pattern Matching       \n";
    std::cout << "==========================================\n\n";

    std::string text = "AABAACAADAABAABA";
    std::string pattern = "AABA";

    std::cout << "Text    : \"" << text << "\"\n";
    std::cout << "Pattern : \"" << pattern << "\"\n\n";

    std::vector<int> matches = searchPatternZ(text, pattern);

    std::cout << "Occurrences found: " << matches.size() << "\n";
    std::cout << "Match starting indices: ";
    for (size_t i = 0; i < matches.size(); ++i) {
        std::cout << matches[i] << (i + 1 < matches.size() ? ", " : "\n");
    }

    std::cout << "\n--- Text Visualization ---\n";
    std::cout << text << "\n";
    std::string marker(text.length(), ' ');
    for (int idx : matches) {
        marker[idx] = '^';
    }
    std::cout << marker << "\n";

    // Second test case
    std::string text2 = "banana";
    std::string pattern2 = "an";
    std::cout << "\nText: \"" << text2 << "\", Pattern: \"" << pattern2 << "\"\n";
    auto matches2 = searchPatternZ(text2, pattern2);
    std::cout << "Indices: ";
    for (int idx : matches2) std::cout << idx << " ";
    std::cout << "\n";

    return 0;
}
