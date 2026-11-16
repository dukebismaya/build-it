/*
 * ============================================================================
 *  Smallest Suffix Array (SA-IS Lite) with LCP via Kasai's Algorithm
 * ============================================================================
 *
 *  A suffix array is a sorted array of all suffixes of a string, represented
 *  as an array of starting indices. Combined with the LCP (Longest Common
 *  Prefix) array, it enables efficient string processing: pattern matching,
 *  counting distinct substrings, longest repeated substring, etc.
 *
 *  This implementation uses a simplified O(N log^2 N) construction:
 *  - Rank suffixes by their first 2^k characters, doubling k each iteration.
 *  - Sort by (rank, rank + 2^k) pairs using std::sort.
 *
 *  Kasai's Algorithm for LCP Array:
 *  - Compute LCP between consecutive suffixes in the suffix array.
 *  - Uses the property: if LCP(SA[i-1], SA[i]) = h, then
 *    LCP(SA[rank[SA[i-1]+1]-1], SA[rank[SA[i-1]+1]]) >= h - 1.
 *  - Runs in O(N) time.
 *
 *  Applications demonstrated:
 *  1) Count distinct substrings = N*(N+1)/2 - sum(LCP).
 *  2) Longest repeated substring.
 *  3) Pattern search in O(M log N) via binary search on SA.
 *
 *  Time Complexity:
 *  - SA construction: O(N log^2 N)
 *  - LCP (Kasai): O(N)
 *  - Pattern search: O(M log N) per pattern
 *  Space Complexity: O(N)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <numeric>

namespace suffix_array {

struct SuffixArrayLCP {
    std::string text;
    std::vector<int> sa;    /* Suffix array */
    std::vector<int> rank_arr; /* Rank of each suffix */
    std::vector<int> lcp;   /* LCP array: lcp[i] = LCP(sa[i-1], sa[i]) */
};

SuffixArrayLCP build(const std::string& s) {
    int n = static_cast<int>(s.size());
    SuffixArrayLCP result;
    result.text = s;
    result.sa.resize(n);
    result.rank_arr.resize(n);

    std::iota(result.sa.begin(), result.sa.end(), 0);

    /* Initial ranking by single character */
    for (int i = 0; i < n; ++i) {
        result.rank_arr[i] = s[i];
    }

    for (int gap = 1; gap < n; gap *= 2) {
        auto rankCopy = result.rank_arr;

        auto cmp = [&](int a, int b) {
            if (rankCopy[a] != rankCopy[b]) return rankCopy[a] < rankCopy[b];
            int ra = (a + gap < n) ? rankCopy[a + gap] : -1;
            int rb = (b + gap < n) ? rankCopy[b + gap] : -1;
            return ra < rb;
        };

        std::sort(result.sa.begin(), result.sa.end(), cmp);

        /* Recompute ranks */
        result.rank_arr[result.sa[0]] = 0;
        for (int i = 1; i < n; ++i) {
            result.rank_arr[result.sa[i]] = result.rank_arr[result.sa[i - 1]];
            if (cmp(result.sa[i - 1], result.sa[i])) {
                result.rank_arr[result.sa[i]]++;
            }
        }

        /* Early termination if all ranks are unique */
        if (result.rank_arr[result.sa[n - 1]] == n - 1) break;
    }

    /* Kasai's algorithm for LCP */
    result.lcp.resize(n, 0);
    int h = 0;
    for (int i = 0; i < n; ++i) {
        if (result.rank_arr[i] > 0) {
            int j = result.sa[result.rank_arr[i] - 1];
            while (i + h < n && j + h < n && s[i + h] == s[j + h]) {
                ++h;
            }
            result.lcp[result.rank_arr[i]] = h;
            if (h > 0) --h;
        } else {
            h = 0;
        }
    }

    return result;
}

/* Count distinct substrings = N*(N+1)/2 - sum(LCP) */
int64_t countDistinctSubstrings(const SuffixArrayLCP& sa) {
    int n = static_cast<int>(sa.text.size());
    int64_t total = static_cast<int64_t>(n) * (n + 1) / 2;
    for (int i = 1; i < n; ++i) {
        total -= sa.lcp[i];
    }
    return total;
}

/* Find longest repeated substring */
std::string longestRepeatedSubstring(const SuffixArrayLCP& sa) {
    int maxLCP = 0;
    int maxIdx = -1;
    for (int i = 1; i < static_cast<int>(sa.lcp.size()); ++i) {
        if (sa.lcp[i] > maxLCP) {
            maxLCP = sa.lcp[i];
            maxIdx = i;
        }
    }
    if (maxIdx == -1) return "";
    return sa.text.substr(sa.sa[maxIdx], maxLCP);
}

/* Binary search for pattern in suffix array */
int searchPattern(const SuffixArrayLCP& sa, const std::string& pattern) {
    int n = static_cast<int>(sa.sa.size());
    int m = static_cast<int>(pattern.size());

    int lo = 0, hi = n - 1, result = -1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        std::string suffix = sa.text.substr(sa.sa[mid],
            std::min(m, n - sa.sa[mid]));
        if (suffix >= pattern) {
            if (suffix.substr(0, m) == pattern) result = sa.sa[mid];
            hi = mid - 1;
        } else {
            lo = mid + 1;
        }
    }
    return result;
}

} // namespace suffix_array

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Suffix Array + LCP (Kasai's Algorithm)\n";
    std::cout << "========================================================\n\n";

    std::string text = "banana";
    auto sa = suffix_array::build(text);

    std::cout << "Text: \"" << text << "\"\n\n";

    std::cout << "Suffix Array with LCP:\n";
    std::cout << std::setw(6) << "Rank" << std::setw(6) << "SA[i]"
              << std::setw(6) << "LCP" << "  Suffix\n";
    std::cout << "-------------------------------\n";

    int n = static_cast<int>(text.size());
    for (int i = 0; i < n; ++i) {
        std::cout << std::setw(6) << i << std::setw(6) << sa.sa[i]
                  << std::setw(6) << sa.lcp[i] << "  "
                  << text.substr(sa.sa[i]) << "\n";
    }

    /* Distinct substrings */
    int64_t distinct = suffix_array::countDistinctSubstrings(sa);
    std::cout << "\nDistinct substrings: " << distinct << "\n";

    /* Longest repeated substring */
    std::string lrs = suffix_array::longestRepeatedSubstring(sa);
    std::cout << "Longest repeated substring: \"" << lrs << "\" (length "
              << lrs.size() << ")\n";

    /* Pattern search */
    std::cout << "\nPattern Search:\n";
    std::vector<std::string> patterns = {"ana", "nan", "ban", "xyz", "a"};
    for (const auto& pat : patterns) {
        int pos = suffix_array::searchPattern(sa, pat);
        std::cout << "  \"" << pat << "\" -> "
                  << (pos >= 0 ? "found at index " + std::to_string(pos) : "not found")
                  << "\n";
    }

    /* Test with a longer string */
    std::cout << "\n--- Longer String Test ---\n";
    std::string text2 = "abracadabra";
    auto sa2 = suffix_array::build(text2);

    std::cout << "Text: \"" << text2 << "\"\n";
    std::cout << "Distinct substrings: " << suffix_array::countDistinctSubstrings(sa2) << "\n";
    std::cout << "Longest repeated: \"" << suffix_array::longestRepeatedSubstring(sa2) << "\"\n";

    return 0;
}
