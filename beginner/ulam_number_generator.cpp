/*
 * ============================================================
 *                 Ulam Number Generator
 * ============================================================
 *
 * Description:
 *   The Ulam sequence starts with U1 = 1, U2 = 2. Each next
 *   term is the smallest integer greater than the previous term
 *   that can be written as the sum of two DISTINCT earlier terms
 *   in exactly ONE way.
 *
 *   1, 2, 3, 4, 6, 8, 11, 13, 16, 18, 26, 28, 36, 38, 47, ...
 *
 * Algorithm:
 *   Maintain ways[v] = number of representations of v as a sum
 *   of two distinct terms found so far. When a new term t is
 *   appended, for every earlier term u: ways[u + t]++.
 *   The next term is the smallest v > last with ways[v] == 1.
 *
 * Complexity:
 *   O(N^2) for N terms (each new term pairs with all earlier),
 *   space O(max term).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <iomanip>

namespace ulam {

    std::vector<int> generate(int count) {
        std::vector<int> seq{1, 2};
        std::vector<int> ways(16, 0);
        ways[3] = 1;
        while (static_cast<int>(seq.size()) < count) {
            int v = seq.back() + 1;
            while (true) {
                if (v >= static_cast<int>(ways.size())) ways.resize(v * 2, 0);
                if (ways[v] == 1) break;
                ++v;
            }
            for (int u : seq) {
                int sum = u + v;
                if (sum >= static_cast<int>(ways.size())) ways.resize(sum * 2, 0);
                if (ways[sum] < 2) ++ways[sum];  // cap: only care about 0/1/many
            }
            seq.push_back(v);
        }
        return seq;
    }

    // Brute-force verification of the uniqueness property
    bool verify(const std::vector<int>& seq) {
        for (size_t k = 2; k < seq.size(); ++k) {
            int reps = 0;
            for (size_t i = 0; i < k; ++i)
                for (size_t j = i + 1; j < k; ++j)
                    if (seq[i] + seq[j] == seq[k]) ++reps;
            if (reps != 1) return false;
        }
        return true;
    }

} // namespace ulam

int main() {
    std::cout << "============================================================\n";
    std::cout << "                  Ulam Number Generator\n";
    std::cout << "============================================================\n\n";

    auto seq = ulam::generate(60);
    std::cout << "First 60 Ulam numbers:\n";
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << std::setw(5) << seq[i];
        if ((i + 1) % 10 == 0) std::cout << "\n";
    }

    std::cout << "\nUniqueness property verified by brute force: "
              << (ulam::verify(seq) ? "PASSED" : "FAILED") << "\n";

    auto big = ulam::generate(2000);
    std::cout << "\nU(2000) = " << big.back()
              << ", density ~ " << std::fixed << std::setprecision(4)
              << 2000.0 / big.back() << " (known limit ~0.0739)\n";
    return 0;
}
