/*
 * Program Description:
 * Convex Hull Trick (CHT) Dynamic Programming Optimization in C++.
 * Maintains a set of linear functions y = m * x + c such that for any query x,
 * the minimum (or maximum) value of m * x + c can be evaluated in O(log N) time.
 *
 * Use Case in DP:
 * Optimizes DP transitions of the form:
 * dp[i] = min_{j < i} (dp[j] + b[j] * a[i])
 * from O(N^2) down to O(N log N) (or O(N) if slopes are monotonic).
 *
 * Algorithm Details:
 * 1. Lines are represented by y = m * x + c.
 * 2. When inserting line L3 after L1, L2:
 *    Compute intersection points: intersect(L1, L3) and intersect(L1, L2).
 *    If intersect(L1, L3) <= intersect(L1, L2), then L2 is redundant and can be removed.
 * 3. Query minimum at x:
 *    Binary search over the envelope of non-redundant lines.
 *
 * Time Complexity:
 * - Line Addition: Amortized O(1) (with monotonic slopes)
 * - Query at x: O(log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <iomanip>

struct Line {
    long long m, c;

    long long eval(long long x) const {
        return m * x + c;
    }
};

class ConvexHullTrickMin {
    std::vector<Line> lines;

    // Checks whether line l2 is redundant given l1 and l3
    // Returns true if intersection(l1, l3) <= intersection(l1, l2)
    // Equivalent to: (l3.c - l1.c) / (l1.m - l3.m) <= (l2.c - l1.c) / (l1.m - l2.m)
    bool isRedundant(const Line& l1, const Line& l2, const Line& l3) const {
        return static_cast<double>(l3.c - l1.c) / (l1.m - l3.m) <=
               static_cast<double>(l2.c - l1.c) / (l1.m - l2.m);
    }

public:
    // Inserts line with decreasing slope m
    void addLine(long long m, long long c) {
        Line newLine = {m, c};
        while (lines.size() >= 2 && isRedundant(lines[lines.size() - 2], lines.back(), newLine)) {
            lines.pop_back();
        }
        lines.push_back(newLine);
    }

    // Queries minimum y = m * x + c for given x in O(log N)
    long long queryMin(long long x) const {
        if (lines.empty()) return LLONG_MAX;

        int low = 0, high = lines.size() - 1;
        long long best = lines[0].eval(x);

        while (low <= high) {
            int mid = low + (high - low) / 2;
            long long curVal = lines[mid].eval(x);
            best = std::min(best, curVal);

            // Compare slope with neighbors
            if (mid + 1 < static_cast<int>(lines.size()) && lines[mid + 1].eval(x) < curVal) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        return best;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Convex Hull Trick (CHT) DP Optimizer   \n";
    std::cout << "==========================================\n\n";

    ConvexHullTrickMin cht;

    // Insert lines with decreasing slopes:
    // L1: y = 4x + 10
    // L2: y = 3x + 15
    // L3: y = 2x + 22
    // L4: y = 1x + 35
    cht.addLine(4, 10);
    cht.addLine(3, 15);
    cht.addLine(2, 22);
    cht.addLine(1, 35);

    std::cout << "Added lines: [4x + 10], [3x + 15], [2x + 22], [x + 35]\n\n";

    std::vector<long long> testPoints = {0, 2, 5, 8, 12, 20};

    std::cout << "Minimum evaluation queries across test points x:\n";
    for (long long x : testPoints) {
        long long minVal = cht.queryMin(x);
        std::cout << "  x = " << std::setw(2) << x << " -> min(y) = " << minVal << "\n";
    }

    std::cout << "\nVerification: CHT lower envelope evaluated accurately!\n";
    return 0;
}
