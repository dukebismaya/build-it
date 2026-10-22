/*
 * Program Description:
 * Activity Selection Problem (Interval Scheduling) using Greedy Strategy.
 * Given a set of N activities with start and finish times, select the maximum number
 * of activities that can be performed by a single person/machine, assuming that a person
 * can only work on a single activity at a time.
 *
 * Greedy Choice Property:
 * Always choose the next compatible activity whose finish time is earliest.
 * This leaves the maximum possible remaining time for subsequent activities.
 *
 * Algorithm:
 * 1. Sort activities in ascending order of their finish times.
 * 2. Select the first activity.
 * 3. For every subsequent activity, if its start time >= finish time of the previously
 *    selected activity, select it and update the last finish time.
 *
 * Time Complexity: O(N log N) due to sorting.
 * Space Complexity: O(N) to store selected activities.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

struct Activity {
    std::string name;
    int start;
    int finish;

    bool operator<(const Activity& other) const {
        return finish < other.finish;
    }
};

std::vector<Activity> selectMaxActivities(std::vector<Activity> activities) {
    if (activities.empty()) return {};

    // Sort by finish time ascending
    std::sort(activities.begin(), activities.end());

    std::vector<Activity> selected;
    selected.push_back(activities[0]);
    int lastFinish = activities[0].finish;

    for (size_t i = 1; i < activities.size(); ++i) {
        if (activities[i].start >= lastFinish) {
            selected.push_back(activities[i]);
            lastFinish = activities[i].finish;
        }
    }

    return selected;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "       Activity Selection (Greedy)        \n";
    std::cout << "==========================================\n\n";

    std::vector<Activity> schedule = {
        {"A1", 1, 2},
        {"A2", 3, 4},
        {"A3", 0, 6},
        {"A4", 5, 7},
        {"A5", 8, 9},
        {"A6", 5, 9}
    };

    std::cout << "Available Activities:\n";
    std::cout << "  Name | Start Time | Finish Time\n";
    std::cout << "  -----+------------+------------\n";
    for (const auto& a : schedule) {
        std::cout << "  " << std::setw(4) << a.name << " | "
                  << std::setw(10) << a.start << " | "
                  << std::setw(11) << a.finish << "\n";
    }

    auto chosen = selectMaxActivities(schedule);

    std::cout << "\nOptimal Non-Overlapping Schedule (" << chosen.size() << " activities):\n";
    for (const auto& a : chosen) {
        std::cout << "  -> " << a.name << ": [" << a.start << " -> " << a.finish << "]\n";
    }

    std::cout << "\nVerification: Selected " << chosen.size() << " activities without conflicts!\n";
    return 0;
}
