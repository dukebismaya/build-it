/**
 * @file boyer_moore_horspool_search.cpp
 * @brief Implements the Boyer-Moore-Horspool string searching algorithm with performance metrics.
 *
 * Background:
 * Published by Nigel Horspool in 1980, the Boyer-Moore-Horspool algorithm simplifies
 * the classic Boyer-Moore algorithm by using only the Bad Character heuristic.
 *
 * Key Insight:
 * Instead of looking at the character where a mismatch occurs, Horspool always determines
 * the shift distance using the character in the text aligned with the *last character*
 * of the pattern.
 *
 * Algorithm:
 * 1. Preprocessing:
 *    - Initialize shift[c] = m (pattern length) for all alphabet characters.
 *    - For j = 0 to m - 2:
 *        shift[pattern[j]] = m - 1 - j
 * 2. Searching:
 *    - Align pattern at text index k = m - 1.
 *    - Compare pattern right-to-left.
 *    - If match, record occurrence.
 *    - Shift window by shift[text[k]].
 *
 * Time Complexity:
 * - Preprocessing: O(M + Sigma) where Sigma is alphabet size.
 * - Average Search: O(N / M) sublinear time.
 * - Worst-Case Search: O(N * M).
 * Space Complexity: O(Sigma) for the shift table.
 */

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>

struct SearchMetrics {
    std::vector<int> matchIndices;
    long long comparisons;
};

class BoyerMooreHorspool {
private:
    std::string pattern;
    int m;
    static constexpr int ALPHABET_SIZE = 256;
    std::vector<int> shiftTable;

    void buildShiftTable() {
        shiftTable.assign(ALPHABET_SIZE, m);
        for (int j = 0; j < m - 1; ++j) {
            unsigned char c = static_cast<unsigned char>(pattern[j]);
            shiftTable[c] = m - 1 - j;
        }
    }

public:
    explicit BoyerMooreHorspool(std::string pat)
        : pattern(std::move(pat)), m(static_cast<int>(pattern.length())), shiftTable(ALPHABET_SIZE) {
        buildShiftTable();
    }

    SearchMetrics search(const std::string& text) const {
        int n = static_cast<int>(text.length());
        SearchMetrics metrics;
        metrics.comparisons = 0;

        if (m == 0 || n < m) return metrics;

        int k = m - 1; // alignment index pointing to the end of the current window

        while (k < n) {
            int i = m - 1;
            int j = k;

            while (i >= 0) {
                metrics.comparisons++;
                if (pattern[i] == text[j]) {
                    --i;
                    --j;
                } else {
                    break;
                }
            }

            if (i < 0) {
                // Entire pattern matched
                metrics.matchIndices.push_back(j + 1);
            }

            // Always shift by the character aligned with the end of the window
            unsigned char windowEndChar = static_cast<unsigned char>(text[k]);
            k += shiftTable[windowEndChar];
        }

        return metrics;
    }

    const std::vector<int>& getShiftTable() const {
        return shiftTable;
    }
};

/**
 * @brief Naive O(N * M) brute-force search for benchmarking comparisons.
 */
SearchMetrics naiveSearch(const std::string& text, const std::string& pattern) {
    int n = static_cast<int>(text.length());
    int m = static_cast<int>(pattern.length());
    SearchMetrics metrics;
    metrics.comparisons = 0;

    for (int i = 0; i <= n - m; ++i) {
        bool match = true;
        for (int j = 0; j < m; ++j) {
            metrics.comparisons++;
            if (text[i + j] != pattern[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            metrics.matchIndices.push_back(i);
        }
    }

    return metrics;
}

int main() {
    std::cout << "=== Boyer-Moore-Horspool String Searching Algorithm ===\n\n";

    std::string text = "THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG AND THE SLEEPING CAT";
    std::vector<std::string> patterns = {"THE", "FOX", "CAT", "NOTFOUND", "E"};

    std::cout << "Text: \"" << text << "\" (Length: " << text.length() << ")\n\n";

    std::cout << "  " << std::setw(12) << "Pattern"
              << std::setw(14) << "Occurrences"
              << std::setw(16) << "Match Indices"
              << std::setw(16) << "BMH Compares"
              << std::setw(16) << "Naive Compares" << '\n';
    std::cout << "  " << std::string(74, '-') << '\n';

    for (const auto& pat : patterns) {
        BoyerMooreHorspool bmh(pat);
        auto bmhResult = bmh.search(text);
        auto naiveResult = naiveSearch(text, pat);

        std::string indicesStr = "[";
        for (size_t i = 0; i < bmhResult.matchIndices.size(); ++i) {
            indicesStr += std::to_string(bmhResult.matchIndices[i]);
            if (i + 1 < bmhResult.matchIndices.size()) indicesStr += ", ";
        }
        indicesStr += "]";

        std::cout << "  " << std::setw(12) << ("\"" + pat + "\"")
                  << std::setw(14) << bmhResult.matchIndices.size()
                  << std::setw(16) << indicesStr
                  << std::setw(16) << bmhResult.comparisons
                  << std::setw(16) << naiveResult.comparisons << '\n';
    }

    return 0;
}
