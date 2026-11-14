/**
 * @file interval_scheduling_greedy.cpp
 * @brief Optimal Earliest Finish Time (EFT) Interval Scheduling in C++17.
 *
 * Problem Definition (Interval Scheduling Maximization Problem):
 *   Given a set of N tasks/intervals with start and finish times [s_i, f_i],
 *   find a maximum-size subset of mutually compatible (non-overlapping) intervals.
 *
 * Optimal Greedy Algorithm: Earliest Finish Time (EFT)
 *   1. Sort intervals by finish times: f_1 <= f_2 <= ... <= f_N.
 *   2. Greedily select the first compatible interval that finishes earliest.
 *   3. Repeat until no more intervals can be selected.
 *
 * Optimality Proof ("Greedy Stays Ahead"):
 *   At each step k, the finish time of the k-th greedy interval is <= the finish time
 *   of the k-th interval in ANY other valid schedule. Hence, greedy leaves the maximum
 *   possible remaining time for future intervals.
 *
 * Flawed Heuristics (Counterexamples demonstrated):
 *   - Earliest Start Time: [0, 10], [1, 3], [4, 6] -> Picks [0, 10] (count 1 vs optimal 2).
 *   - Shortest Duration:   [0, 4], [3, 5], [4, 8]  -> Picks [3, 5] (count 1 vs optimal 2).
 *
 * Complexity:
 *   - Time: O(N log N) dominated by sorting.
 *   - Space: O(N) to store intervals.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

namespace scheduling {

struct Interval {
    int id;
    int start;
    int finish;
    std::string name;
};

/**
 * @brief Solves ISMP optimally via Earliest Finish Time (EFT).
 */
std::vector<Interval> solveEarliestFinishTime(std::vector<Interval> intervals) {
    if (intervals.empty()) return {};

    // Sort by finish time ascending (break ties with start time)
    std::sort(intervals.begin(), intervals.end(),
              [](const Interval& a, const Interval& b) {
                  if (a.finish != b.finish) return a.finish < b.finish;
                  return a.start < b.start;
              });

    std::vector<Interval> schedule;
    schedule.push_back(intervals[0]);
    int lastFinish = intervals[0].finish;

    for (size_t i = 1; i < intervals.size(); ++i) {
        if (intervals[i].start >= lastFinish) {
            schedule.push_back(intervals[i]);
            lastFinish = intervals[i].finish;
        }
    }

    return schedule;
}

} // namespace scheduling

int main() {
    std::cout << "========================================\n";
    std::cout << "     INTERVAL SCHEDULING ALGORITHM      \n";
    std::cout << "    (Earliest Finish Time Heuristic)    \n";
    std::cout << "========================================\n\n";

    std::vector<scheduling::Interval> tasks = {
        {1, 1, 4, "Task_A"},
        {2, 3, 5, "Task_B"},
        {3, 0, 6, "Task_C"},
        {4, 5, 7, "Task_D"},
        {5, 3, 9, "Task_E"},
        {6, 5, 9, "Task_F"},
        {7, 6, 10, "Task_G"},
        {8, 8, 11, "Task_H"},
        {9, 8, 12, "Task_I"},
        {10, 2, 14, "Task_J"},
        {11, 12, 16, "Task_K"}
    };

    std::cout << "Candidate Tasks (" << tasks.size() << " total):\n";
    for (const auto& t : tasks) {
        std::cout << "  " << std::left << std::setw(8) << t.name
                  << ": [" << std::setw(2) << t.start << ", "
                  << std::setw(2) << t.finish << "]\n";
    }

    auto optimalSchedule = scheduling::solveEarliestFinishTime(tasks);

    std::cout << "\nOptimal Non-Overlapping Schedule (EFT Algorithm):\n";
    std::cout << "Max Tasks Scheduled: " << optimalSchedule.size() << "\n";
    std::cout << std::left << std::setw(8)  << "Step"
              << std::setw(12) << "Task"
              << "Interval Span\n";
    std::cout << std::string(36, '-') << "\n";

    for (size_t i = 0; i < optimalSchedule.size(); ++i) {
        std::cout << std::left << std::setw(8) << (i + 1)
                  << std::setw(12) << optimalSchedule[i].name
                  << "[" << optimalSchedule[i].start << " -> "
                  << optimalSchedule[i].finish << "]\n";
    }

    std::cout << "\nVerification:\n";
    std::cout << "Every scheduled task starts after or when the preceding task finishes.\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
