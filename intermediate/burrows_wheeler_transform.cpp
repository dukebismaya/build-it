/**
 * @file burrows_wheeler_transform.cpp
 * @brief Forward and Inverse Burrows-Wheeler Transform (BWT) via LF-mapping in C++17.
 *
 * Overview (Burrows & Wheeler, 1994):
 *   The Burrows-Wheeler Transform rearranges a string into runs of similar characters,
 *   making it highly compressible (e.g. in bzip2 and FM-indexes for DNA alignment).
 *
 * Forward Transform:
 *   1. Append a unique sentinel character '$' (lexicographically smallest).
 *   2. Form all cyclic rotations of the string.
 *   3. Sort the cyclic rotations lexicographically.
 *   4. Output the last column L of the sorted rotation matrix.
 *
 * Inverse Transform (LF Mapping):
 *   Reconstructs the original string in O(N) time and O(N) space using the
 *   "Last-to-First" property: The k-th occurrence of character C in the last column L
 *   corresponds to the k-th occurrence of C in the first column F (which is simply L sorted).
 *
 * Complexity:
 *   - Forward Transform: O(N^2 log N) naive, or O(N log N) using Suffix Array.
 *   - Inverse Transform: O(N * alphabet) time, O(N) space.
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <map>

namespace compression {

struct BWTResult {
    std::string bwtString;
    size_t primaryIndex; // Index of the original string in sorted rotations
};

/**
 * @brief Computes the forward Burrows-Wheeler Transform of text (appends '$' sentinel).
 */
BWTResult forwardBWT(std::string text) {
    text += '$';
    size_t n = text.length();

    // Generate all cyclic rotations
    std::vector<std::string> rotations(n);
    for (size_t i = 0; i < n; ++i) {
        rotations[i] = text.substr(i) + text.substr(0, i);
    }

    std::sort(rotations.begin(), rotations.end());

    std::string lastCol;
    lastCol.reserve(n);
    size_t primaryIdx = 0;

    for (size_t i = 0; i < n; ++i) {
        lastCol += rotations[i].back();
        if (rotations[i] == text) {
            primaryIdx = i;
        }
    }

    return {lastCol, primaryIdx};
}

/**
 * @brief Inverts the BWT using the Last-to-First (LF) mapping.
 */
std::string inverseBWT(const std::string& bwt) {
    size_t n = bwt.length();
    if (n == 0) return "";

    // Count frequencies of each character in L
    std::map<char, int> count;
    std::vector<int> occurrence(n);
    for (size_t i = 0; i < n; ++i) {
        occurrence[i] = count[bwt[i]]++;
    }

    // Determine starting position of each character in sorted first column F
    std::map<char, int> startPos;
    int cumulative = 0;
    for (auto& pair : count) {
        startPos[pair.first] = cumulative;
        cumulative += pair.second;
    }

    // Build LF mapping: LF[i] is the row index in F corresponding to L[i]
    std::vector<int> lf(n);
    for (size_t i = 0; i < n; ++i) {
        lf[i] = startPos[bwt[i]] + occurrence[i];
    }

    // Reconstruct backwards starting from sentinel '$'
    int curr = 0;
    for (size_t i = 0; i < n; ++i) {
        if (bwt[i] == '$') {
            curr = static_cast<int>(i);
            break;
        }
    }

    std::string original;
    original.reserve(n);

    for (size_t step = 0; step < n - 1; ++step) {
        curr = lf[curr];
        original += bwt[curr];
    }

    std::reverse(original.begin(), original.end());
    return original;
}

} // namespace compression

int main() {
    std::cout << "========================================\n";
    std::cout << "     BURROWS-WHEELER TRANSFORM (BWT)    \n";
    std::cout << "    (Forward & Inverse via LF Mapping)  \n";
    std::cout << "========================================\n\n";

    std::vector<std::string> sampleTexts = {
        "banana",
        "mississippi",
        "abracadabra",
        "panamabanana"
    };

    for (const auto& text : sampleTexts) {
        auto bwtRes = compression::forwardBWT(text);
        std::string recovered = compression::inverseBWT(bwtRes.bwtString);

        std::cout << "Original Text : \"" << text << "\"\n";
        std::cout << "  - BWT Output: \"" << bwtRes.bwtString << "\"\n";
        std::cout << "  - Recovered : \"" << recovered << "\"\n";
        std::cout << "  - Lossless? : " << (text == recovered ? "YES (Perfect Match)" : "FAIL") << "\n\n";
    }

    std::cout << "Program finished successfully.\n";
    return 0;
}
