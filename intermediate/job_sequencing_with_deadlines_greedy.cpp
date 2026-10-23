/*
 * Program Description:
 * Job Sequencing with Deadlines using Greedy Algorithm and DSU.
 * Given a set of N jobs where each job takes 1 unit of time, has a deadline and profit,
 * schedule the jobs to maximize total profit such that each job completes on or before its deadline.
 *
 * Algorithm Strategy:
 * 1. Sort all jobs in descending order of their profit (Greedy Choice).
 * 2. For each job, schedule it at the latest available time slot <= deadline.
 * 3. Use Disjoint Set Union (DSU) to find the latest available slot in near O(1) time
 *    (Ackermann inverse alpha):
 *    - parent[slot] points to the latest free slot before or at that slot.
 *
 * Time Complexity: O(N log N + N * alpha(maxDeadline))
 * Space Complexity: O(maxDeadline).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <iomanip>

struct Job {
    std::string id;
    int deadline;
    int profit;

    Job(std::string name, int d, int p) : id(name), deadline(d), profit(p) {}
};

class DSU {
    std::vector<int> parent;

public:
    explicit DSU(int n) : parent(n + 1) {
        for (int i = 0; i <= n; ++i) {
            parent[i] = i;
        }
    }

    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }

    // Connects slot u to slot u - 1
    void unite(int u, int v) {
        parent[u] = v;
    }
};

struct ScheduleResult {
    int totalProfit;
    int jobCount;
    std::vector<std::pair<int, std::string>> timeline; // <slot, jobID>
};

ScheduleResult scheduleJobs(std::vector<Job> jobs) {
    if (jobs.empty()) return {0, 0, {}};

    // Sort jobs by profit descending
    std::sort(jobs.begin(), jobs.end(), [](const Job& a, const Job& b) {
        return a.profit > b.profit;
    });

    int maxDeadline = 0;
    for (const auto& job : jobs) {
        maxDeadline = std::max(maxDeadline, job.deadline);
    }

    DSU dsu(maxDeadline);
    int totalProfit = 0;
    int count = 0;
    std::vector<std::pair<int, std::string>> timeline;

    for (const auto& job : jobs) {
        // Find latest available slot <= job.deadline
        int availableSlot = dsu.find(job.deadline);

        if (availableSlot > 0) {
            // Assign job to availableSlot
            totalProfit += job.profit;
            count++;
            timeline.push_back({availableSlot, job.id});

            // Mark this slot as filled by pointing to the previous slot
            dsu.unite(availableSlot, dsu.find(availableSlot - 1));
        }
    }

    std::sort(timeline.begin(), timeline.end());
    return {totalProfit, count, timeline};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Job Sequencing with Deadlines (DSU)   \n";
    std::cout << "==========================================\n\n";

    std::vector<Job> jobs = {
        Job("J1", 4, 70),
        Job("J2", 1, 80),
        Job("J3", 1, 30),
        Job("J4", 1, 100),
        Job("J5", 2, 60),
        Job("J6", 3, 50)
    };

    std::cout << "Available Jobs:\n";
    std::cout << "  Job ID | Deadline | Profit\n";
    std::cout << "  -------+----------+-------\n";
    for (const auto& j : jobs) {
        std::cout << "  " << std::setw(6) << j.id << " | "
                  << std::setw(8) << j.deadline << " | "
                  << std::setw(6) << j.profit << "\n";
    }

    auto schedule = scheduleJobs(jobs);

    std::cout << "\n--- Optimal Schedule ---\n";
    std::cout << "Jobs scheduled : " << schedule.jobCount << "\n";
    std::cout << "Total Profit   : $" << schedule.totalProfit << "\n";
    std::cout << "Timeline Execution Slots:\n";
    for (const auto& [slot, jobId] : schedule.timeline) {
        std::cout << "  Time Slot [" << slot - 1 << " - " << slot << "]: " << jobId << "\n";
    }

    return 0;
}
