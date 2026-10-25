/*
 * Program Description:
 * Collatz Conjecture (3n + 1 Hailstone Sequence) Analyzer in C++.
 * Given any positive integer N:
 * - If N is even: N = N / 2
 * - If N is odd : N = 3 * N + 1
 * The conjecture posits that this sequence always reaches 1 for all positive integers.
 *
 * Metrics Computed:
 * 1. Trajectory generation (full path to 1).
 * 2. Total Stopping Time (number of transitions).
 * 3. Peak Altitude (maximum value attained during sequence).
 * 4. Range benchmark finding the longest sequence in [1, MaxN].
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct CollatzReport {
    long long start;
    int steps;
    long long peakValue;
    std::vector<long long> trajectory;
};

CollatzReport analyzeCollatz(long long n) {
    CollatzReport report;
    report.start = n;
    report.steps = 0;
    report.peakValue = n;

    if (n <= 0) return report;

    long long current = n;
    report.trajectory.push_back(current);

    while (current != 1) {
        if (current % 2 == 0) {
            current /= 2;
        } else {
            current = 3 * current + 1;
        }
        report.trajectory.push_back(current);
        report.steps++;
        if (current > report.peakValue) {
            report.peakValue = current;
        }
    }

    return report;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Collatz Sequence Analyzer         \n";
    std::cout << "==========================================\n\n";

    std::vector<long long> sampleNumbers = {6, 11, 27};

    for (long long num : sampleNumbers) {
        auto rep = analyzeCollatz(num);
        std::cout << "Number: " << rep.start << "\n";
        std::cout << "  Total Steps : " << rep.steps << "\n";
        std::cout << "  Peak Value  : " << rep.peakValue << "\n";
        std::cout << "  Trajectory  : ";
        for (size_t i = 0; i < rep.trajectory.size(); ++i) {
            if (i > 0 && i % 10 == 0) std::cout << "\n                ";
            std::cout << rep.trajectory[i] << (i + 1 < rep.trajectory.size() ? " -> " : "");
        }
        std::cout << "\n\n";
    }

    // Benchmark: Longest Collatz chain for numbers under 100
    int maxSteps = 0;
    long long bestNumber = 1;
    for (int i = 1; i <= 100; ++i) {
        auto rep = analyzeCollatz(i);
        if (rep.steps > maxSteps) {
            maxSteps = rep.steps;
            bestNumber = i;
        }
    }

    std::cout << "--- Range [1..100] Champion ---\n";
    std::cout << "Number with longest sequence: " << bestNumber
              << " with " << maxSteps << " steps!\n";

    return 0;
}
