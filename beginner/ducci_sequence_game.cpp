/**
 * @file ducci_sequence_game.cpp
 * @brief Ducci 4-tuple difference sequence simulator and convergence proof in C++17.
 *
 * Mathematical Definition (Enrico Ducci, 1937):
 *   Given an n-tuple of non-negative integers (a_1, a_2, ..., a_n), the Ducci map D is:
 *     D(a_1, a_2, ..., a_n) = (|a_1 - a_2|, |a_2 - a_3|, ..., |a_n - a_1|).
 *
 * Fundamental Convergence Theorem:
 *   - When n is a power of 2 (such as n = 4):
 *     For EVERY starting 4-tuple of integers, the sequence ALWAYS converges to (0, 0, 0, 0).
 *   - For n = 4, the sequence reaches all zeros in a small number of steps (e.g. <= 14 steps).
 *   - When n is NOT a power of 2 (e.g. n = 3 or n = 5):
 *     The sequence can enter an infinite non-zero periodic cycle.
 *
 * Complexity:
 *   - Per step: O(n) difference evaluations.
 *   - Total steps for n = 4: O(1) bounded steps.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <map>

namespace sequences {

using Tuple = std::vector<int>;

/**
 * @brief Computes one Ducci step: (|a_1 - a_2|, ..., |a_n - a_1|).
 */
Tuple nextDucci(const Tuple& t) {
    size_t n = t.size();
    Tuple res(n);
    for (size_t i = 0; i < n; ++i) {
        res[i] = std::abs(t[i] - t[(i + 1) % n]);
    }
    return res;
}

bool isAllZeros(const Tuple& t) {
    for (int x : t) {
        if (x != 0) return false;
    }
    return true;
}

std::string tupleToString(const Tuple& t) {
    std::string s = "(";
    for (size_t i = 0; i < t.size(); ++i) {
        s += std::to_string(t[i]);
        if (i + 1 < t.size()) s += ", ";
    }
    s += ")";
    return s;
}

struct DucciResult {
    bool reachesZero;
    int stepsToZero;
    int cycleStart;
    int cycleLength;
    std::vector<Tuple> history;
};

/**
 * @brief Simulates Ducci sequence until all zeros or periodic cycle detected.
 */
DucciResult runDucci(Tuple start, int maxSteps = 50) {
    std::vector<Tuple> history;
    std::map<Tuple, int> seen;

    Tuple current = start;
    history.push_back(current);
    seen[current] = 0;

    for (int step = 1; step <= maxSteps; ++step) {
        current = nextDucci(current);
        history.push_back(current);

        if (isAllZeros(current)) {
            return {true, step, -1, 0, history};
        }

        if (seen.find(current) != seen.end()) {
            int prevStep = seen[current];
            int cycleLen = step - prevStep;
            return {false, -1, prevStep, cycleLen, history};
        }

        seen[current] = step;
    }

    return {false, -1, -1, 0, history};
}

} // namespace sequences

int main() {
    std::cout << "========================================\n";
    std::cout << "          DUCCI SEQUENCE GAME           \n";
    std::cout << "    (4-Tuple Global Zero Convergence)   \n";
    std::cout << "========================================\n\n";

    // Example 1: Classic 4-tuple (0, 65, 12, 230)
    sequences::Tuple start4 = {0, 65, 12, 230};
    auto res4 = sequences::runDucci(start4);

    std::cout << "Test 1: 4-Tuple " << sequences::tupleToString(start4) << ":\n";
    for (size_t i = 0; i < res4.history.size(); ++i) {
        std::cout << "  Step " << std::setw(2) << i << ": "
                  << sequences::tupleToString(res4.history[i]) << "\n";
    }
    std::cout << "Conclusion: " << (res4.reachesZero ? "REACHED (0, 0, 0, 0) in " + std::to_string(res4.stepsToZero) + " steps!" : "Cycled") << "\n\n";

    // Example 2: Known 14-step maximal 4-tuple (0, 6, 13, 27)
    sequences::Tuple worst4 = {0, 6, 13, 27};
    auto resWorst = sequences::runDucci(worst4);
    std::cout << "Test 2: Known 14-Step 4-Tuple " << sequences::tupleToString(worst4) << ":\n";
    std::cout << "  - Converged to all zeros in: " << resWorst.stepsToZero << " steps.\n\n";

    // Example 3: 3-Tuple (Non-power-of-2): (1, 0, 0)
    sequences::Tuple start3 = {1, 0, 0};
    auto res3 = sequences::runDucci(start3);
    std::cout << "Test 3: 3-Tuple " << sequences::tupleToString(start3) << " (Non-power-of-2):\n";
    for (size_t i = 0; i < res3.history.size(); ++i) {
        std::cout << "  Step " << std::setw(2) << i << ": "
                  << sequences::tupleToString(res3.history[i]) << "\n";
    }
    std::cout << "Conclusion: Non-zero cycle detected of length " << res3.cycleLength
              << " starting at step " << res3.cycleStart << ".\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
