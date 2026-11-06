/**
 * @file radix_sort_msd_string.cpp
 * @brief Implements Most Significant Digit (MSD) Radix Sort for variable-length strings in O(N * L) time.
 *
 * Algorithm:
 * 1. MSD Radix Sort sorts strings from left to right, inspecting character at index d.
 * 2. Uses counting sort with 256 buckets (ASCII) plus 1 special bucket (-1) for strings that terminate at index d.
 * 3. Partitions strings into sub-buckets based on their character at position d.
 * 4. Recursively sorts each non-trivial bucket on character d + 1.
 * 5. Switches to Insertion Sort when subarray size falls below a threshold (CUTOFF = 15)
 *    to avoid excessive recursion overhead.
 *
 * Time Complexity: O(N * L) where N is string count and L is average string length
 * Space Complexity: O(N + R) where R is alphabet radix (R = 256)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

class MSDRadixSort {
private:
    static constexpr int R = 256;       // Extended ASCII alphabet size
    static constexpr int CUTOFF = 15;   // Cutoff threshold to insertion sort

    static int charAt(const std::string& s, int d) {
        if (d < static_cast<int>(s.length())) {
            return static_cast<unsigned char>(s[d]);
        }
        return -1; // End of string indicator
    }

    static void insertionSort(std::vector<std::string>& a, int lo, int hi, int d) {
        for (int i = lo; i <= hi; ++i) {
            for (int j = i; j > lo && less(a[j], a[j - 1], d); --j) {
                std::swap(a[j], a[j - 1]);
            }
        }
    }

    static bool less(const std::string& v, const std::string& w, int d) {
        size_t minLen = std::min(v.length(), w.length());
        for (size_t i = static_cast<size_t>(d); i < minLen; ++i) {
            if (v[i] < w[i]) return true;
            if (v[i] > w[i]) return false;
        }
        return v.length() < w.length();
    }

    static void sortRecursive(std::vector<std::string>& a, int lo, int hi, int d,
                              std::vector<std::string>& aux) {
        // Cutoff to insertion sort for small subarrays
        if (hi <= lo + CUTOFF) {
            insertionSort(a, lo, hi, d);
            return;
        }

        // Count frequencies of each character at depth d
        // count[0] for terminated strings (-1)
        // count[r + 2] for char r
        std::vector<int> count(R + 2, 0);

        for (int i = lo; i <= hi; ++i) {
            int c = charAt(a[i], d);
            count[c + 2]++;
        }

        // Transform counts to cumulative indices
        for (int r = 0; r < R + 1; ++r) {
            count[r + 1] += count[r];
        }

        // Distribute elements into auxiliary array
        for (int i = lo; i <= hi; ++i) {
            int c = charAt(a[i], d);
            aux[count[c + 1]++] = std::move(a[i]);
        }

        // Copy back to original array
        for (int i = lo; i <= hi; ++i) {
            a[i] = std::move(aux[i - lo]);
        }

        // Recursively sort sub-buckets for each character
        for (int r = 0; r < R; ++r) {
            int subLo = lo + count[r];
            int subHi = lo + count[r + 1] - 1;
            if (subHi > subLo) {
                sortRecursive(a, subLo, subHi, d + 1, aux);
            }
        }
    }

public:
    static void sort(std::vector<std::string>& a) {
        if (a.size() <= 1) return;
        std::vector<std::string> aux(a.size());
        sortRecursive(a, 0, static_cast<int>(a.size()) - 1, 0, aux);
    }
};

int main() {
    std::cout << "=== Most Significant Digit (MSD) Radix Sort for Strings ===\n\n";

    std::vector<std::string> words = {
        "she", "sells", "seashells", "by", "the", "seashore",
        "the", "shells", "she", "sells", "are", "surely", "seashells",
        "algorithm", "zebra", "apple", "banana", "cat", "dog"
    };

    std::cout << "Original String List (Size " << words.size() << "):\n  ";
    for (size_t i = 0; i < words.size(); ++i) {
        std::cout << "\"" << words[i] << "\"";
        if (i + 1 < words.size()) std::cout << ", ";
    }
    std::cout << "\n\n";

    MSDRadixSort::sort(words);

    std::cout << "Lexicographically Sorted List via MSD Radix Sort:\n  ";
    for (size_t i = 0; i < words.size(); ++i) {
        std::cout << "\"" << words[i] << "\"";
        if (i + 1 < words.size()) std::cout << ", ";
    }
    std::cout << "\n\n";

    // Verify sortedness
    bool isSorted = std::is_sorted(words.begin(), words.end());
    std::cout << "Verification: " << (isSorted ? "PASS (Properly Sorted)" : "FAIL") << '\n';

    return 0;
}
