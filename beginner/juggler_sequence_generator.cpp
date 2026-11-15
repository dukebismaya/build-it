/*
 * ============================================================================
 *  Juggler Sequence Generator
 * ============================================================================
 *
 *  The Juggler sequence starts from a positive integer n and repeatedly applies:
 *    a(k+1) = floor(a(k)^(1/2))   if a(k) is even
 *    a(k+1) = floor(a(k)^(3/2))   if a(k) is odd
 *
 *  Every known Juggler sequence eventually reaches 1. The sequence often
 *  "juggles" — rising to large values before crashing down to 1.
 *
 *  This program generates the Juggler sequence for a given starting value,
 *  reports the number of steps to reach 1, and finds the maximum value reached.
 *
 *  Time Complexity: O(steps) per sequence, where steps varies unpredictably.
 *  Space Complexity: O(steps) to store the full sequence.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <iomanip>

namespace juggler {

struct SequenceResult {
    std::vector<int64_t> terms;
    int64_t maxValue;
    int steps;
};

SequenceResult generate(int64_t start) {
    SequenceResult result;
    result.maxValue = start;

    int64_t current = start;
    result.terms.push_back(current);

    while (current != 1) {
        if (current % 2 == 0) {
            current = static_cast<int64_t>(std::floor(std::sqrt(static_cast<double>(current))));
        } else {
            double val = static_cast<double>(current);
            current = static_cast<int64_t>(std::floor(val * std::sqrt(val)));
        }
        result.terms.push_back(current);
        result.maxValue = std::max(result.maxValue, current);
    }

    result.steps = static_cast<int>(result.terms.size()) - 1;
    return result;
}

} // namespace juggler

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Juggler Sequence Generator\n";
    std::cout << "========================================================\n\n";

    /* Demonstrate a few interesting starting values */
    std::vector<int64_t> examples = {3, 5, 9, 25, 37, 77};

    for (int64_t start : examples) {
        auto result = juggler::generate(start);

        std::cout << "Start = " << start << " | Steps = " << result.steps
                  << " | Max = " << result.maxValue << "\n  Sequence: ";

        for (size_t i = 0; i < result.terms.size(); ++i) {
            std::cout << result.terms[i];
            if (i + 1 < result.terms.size()) std::cout << " -> ";
        }
        std::cout << "\n\n";
    }

    /* Find the starting value in [2, 100] with the most steps */
    std::cout << "Survey: longest Juggler sequences for starting values in [2, 100]:\n";
    std::cout << std::setw(8) << "Start" << std::setw(10) << "Steps"
              << std::setw(16) << "Max Value" << "\n";
    std::cout << "------------------------------------\n";

    int64_t bestStart = 2;
    int bestSteps = 0;

    for (int64_t n = 2; n <= 100; ++n) {
        auto result = juggler::generate(n);
        if (result.steps > bestSteps) {
            bestSteps = result.steps;
            bestStart = n;
        }
    }

    /* Show top 5 by step count */
    std::vector<std::pair<int, int64_t>> all;
    for (int64_t n = 2; n <= 100; ++n) {
        auto result = juggler::generate(n);
        all.push_back({result.steps, n});
    }
    std::sort(all.rbegin(), all.rend());

    for (int i = 0; i < 5 && i < static_cast<int>(all.size()); ++i) {
        auto result = juggler::generate(all[i].second);
        std::cout << std::setw(8) << all[i].second << std::setw(10) << all[i].first
                  << std::setw(16) << result.maxValue << "\n";
    }

    std::cout << "\nLongest sequence starts at " << bestStart
              << " with " << bestSteps << " steps.\n";

    return 0;
}
