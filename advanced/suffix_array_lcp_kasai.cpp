/*
 * Program Description:
 * Suffix Array Construction and Kasai's LCP (Longest Common Prefix) Algorithm.
 *
 * Concepts:
 * 1. Suffix Array (SA):
 *    An array of integers giving the starting indices of all suffixes of string S,
 *    sorted in lexicographical order.
 *    Constructed here in O(N log^2 N) using prefix doubling and rank sorting.
 * 2. Kasai's Algorithm for LCP Array:
 *    Computes the LCP between adjacent sorted suffixes in the suffix array in linear O(N) time.
 * 3. Applications:
 *    - Number of distinct substrings: N * (N + 1) / 2 - Sum(LCP[i]).
 *    - Longest Repeated Substring: max element in LCP array.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

struct SuffixInfo {
    int index;
    int rank[2];
};

bool compareSuffixes(const SuffixInfo& a, const SuffixInfo& b) {
    if (a.rank[0] != b.rank[0]) {
        return a.rank[0] < b.rank[0];
    }
    return a.rank[1] < b.rank[1];
}

// Builds the Suffix Array using prefix doubling in O(N log^2 N)
std::vector<int> buildSuffixArray(const std::string& s) {
    int n = s.length();
    std::vector<SuffixInfo> suffixes(n);

    for (int i = 0; i < n; ++i) {
        suffixes[i].index = i;
        suffixes[i].rank[0] = static_cast<unsigned char>(s[i]);
        suffixes[i].rank[1] = (i + 1 < n) ? static_cast<unsigned char>(s[i + 1]) : -1;
    }

    std::sort(suffixes.begin(), suffixes.end(), compareSuffixes);

    std::vector<int> ind(n);
    for (int k = 4; k < 2 * n; k *= 2) {
        int rank = 0;
        int prevRank = suffixes[0].rank[0];
        suffixes[0].rank[0] = rank;
        ind[suffixes[0].index] = 0;

        for (int i = 1; i < n; ++i) {
            if (suffixes[i].rank[0] == prevRank && suffixes[i].rank[1] == suffixes[i - 1].rank[1]) {
                suffixes[i].rank[0] = rank;
            } else {
                prevRank = suffixes[i].rank[0];
                suffixes[i].rank[0] = ++rank;
            }
            ind[suffixes[i].index] = i;
        }

        for (int i = 0; i < n; ++i) {
            int nextIndex = suffixes[i].index + k / 2;
            suffixes[i].rank[1] = (nextIndex < n) ? suffixes[ind[nextIndex]].rank[0] : -1;
        }

        std::sort(suffixes.begin(), suffixes.end(), compareSuffixes);
    }

    std::vector<int> sa(n);
    for (int i = 0; i < n; ++i) {
        sa[i] = suffixes[i].index;
    }
    return sa;
}

// Kasai's algorithm to construct LCP array in O(N)
std::vector<int> buildLCP(const std::string& s, const std::vector<int>& sa) {
    int n = s.length();
    std::vector<int> lcp(n, 0);
    std::vector<int> invSA(n, 0);

    for (int i = 0; i < n; ++i) {
        invSA[sa[i]] = i;
    }

    int k = 0;
    for (int i = 0; i < n; ++i) {
        if (invSA[i] == n - 1) {
            k = 0;
            continue;
        }

        int j = sa[invSA[i] + 1];

        while (i + k < n && j + k < n && s[i + k] == s[j + k]) {
            k++;
        }

        lcp[invSA[i]] = k;

        if (k > 0) {
            k--;
        }
    }

    return lcp;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "  Suffix Array & Kasai's LCP Algorithm    \n";
    std::cout << "==========================================\n\n";

    std::string text = "banana";
    int n = text.length();

    std::cout << "Target String: \"" << text << "\" (length " << n << ")\n\n";

    std::vector<int> sa = buildSuffixArray(text);
    std::vector<int> lcp = buildLCP(text, sa);

    std::cout << "Idx | SA[i] | LCP[i] | Suffix\n";
    std::cout << "----+-------+--------+-----------------\n";
    for (int i = 0; i < n; ++i) {
        std::cout << std::setw(3) << i << " | "
                  << std::setw(5) << sa[i] << " | "
                  << std::setw(6) << lcp[i] << " | "
                  << text.substr(sa[i]) << "\n";
    }

    // Number of distinct substrings
    long long totalSubstrings = static_cast<long long>(n) * (n + 1) / 2;
    long long lcpSum = 0;
    for (int val : lcp) lcpSum += val;
    long long distinctSubstrings = totalSubstrings - lcpSum;

    std::cout << "\nDistinct Substrings: " << distinctSubstrings << " (Expected for \"banana\": 15)\n";

    // Longest repeated substring
    int maxLcp = 0, maxIdx = -1;
    for (int i = 0; i < n; ++i) {
        if (lcp[i] > maxLcp) {
            maxLcp = lcp[i];
            maxIdx = sa[i];
        }
    }
    if (maxIdx != -1) {
        std::cout << "Longest Repeated Substring: \"" << text.substr(maxIdx, maxLcp)
                  << "\" (length " << maxLcp << ")\n";
    }

    return 0;
}
