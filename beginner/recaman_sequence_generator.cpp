/**
 * @file recaman_sequence_generator.cpp
 * @brief Recamán's Sequence generator and step history analyzer in C++17.
 *
 * Definition:
 *   Named after Colombian mathematician Bernardo Recamán Santos, the sequence is defined as:
 *     a_0 = 0
 *     For n > 0:
 *       a_n = a_{n-1} - n,   if (a_{n-1} - n > 0) AND (a_{n-1} - n) is not already in the sequence
 *       a_n = a_{n-1} + n,   otherwise.
 *
 * The sequence begins:
 *   0, 1, 3, 6, 2, 7, 13, 20, 12, 21, 11, 22, 10, 23, 9, 24, 8, 25, 43, 62, ...
 *
 * Properties:
 *   - Values can repeat, though only when jumping forward (+n).
 *   - Famous Open Problem: Does every non-negative integer eventually appear?
 *     (Numbers like 4 and 19 take many steps to appear, e.g. a_131 = 4).
 *
 * Complexity:
 *   - Time: O(N) expected using an unordered_set for O(1) lookup of seen values.
 *   - Space: O(N) to store generated terms and the visited set.
 */

#include <iostream>
#include <vector>
#include <unordered_set>
#include <iomanip>
#include <string>

namespace sequences {

struct RecamanStep {
    int index;
    int value;
    std::string direction;
};

/**
 * @brief Generates the first count terms of Recamán's sequence.
 */
std::vector<RecamanStep> generateRecaman(int count) {
    std::vector<RecamanStep> seq;
    if (count <= 0) return seq;

    std::unordered_set<int> seen;
    int current = 0;
    seq.push_back({0, 0, "START"});
    seen.insert(0);

    for (int n = 1; n < count; ++n) {
        int backward = current - n;
        if (backward > 0 && seen.find(backward) == seen.end()) {
            current = backward;
            seq.push_back({n, current, "-" + std::to_string(n) + " (Back)"});
        } else {
            current = current + n;
            seq.push_back({n, current, "+" + std::to_string(n) + " (Fwd)"});
        }
        seen.insert(current);
    }
    return seq;
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "      RECAMAN'S SEQUENCE GENERATOR      \n";
    std::cout << "========================================\n\n";

    int numTerms = 25;
    auto steps = sequences::generateRecaman(numTerms);

    std::cout << "First " << numTerms << " terms of Recaman's sequence:\n";
    std::cout << std::left << std::setw(8)  << "Step n"
              << std::setw(10) << "a_n"
              << "Jump Direction\n";
    std::cout << std::string(38, '-') << "\n";

    for (const auto& s : steps) {
        std::cout << std::left << std::setw(8)  << s.index
                  << std::setw(10) << s.value
                  << s.direction << "\n";
    }

    // Check which numbers in [0..20] have appeared within 25 steps
    std::unordered_set<int> appeared;
    for (const auto& s : steps) appeared.insert(s.value);

    std::cout << "\nSmall numbers [0..20] NOT yet seen in first " << numTerms << " steps: ";
    bool first = true;
    for (int x = 0; x <= 20; ++x) {
        if (appeared.find(x) == appeared.end()) {
            if (!first) std::cout << ", ";
            std::cout << x;
            first = false;
        }
    }
    std::cout << "\n\nProgram finished successfully.\n";
    return 0;
}
