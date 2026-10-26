/*
 * Program Description:
 * Interval Tree Data Structure in C++.
 * An augmented Binary Search Tree where each node maintains an interval [low, high]
 * and the maximum high value among all intervals in its subtree (maxHigh).
 *
 * Capabilities in O(log N):
 * 1. Insert interval [low, high].
 * 2. Overlap Search: Find any interval in the tree that overlaps with query interval [qLow, qHigh].
 *    Two intervals [A, B] and [C, D] overlap iff A <= D and C <= B.
 * 3. Find All Overlaps: Recursively collect all intersecting intervals.
 *
 * Traversal Strategy for Overlap Query:
 * - If current node overlaps, record/return it.
 * - If left child exists and left->maxHigh >= qLow, the overlapping interval COULD be in left subtree.
 * - Otherwise, it must be in the right subtree.
 *
 * Time Complexity:
 * - Insertion: O(log N)
 * - Overlap Search: O(log N)
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Interval {
    int low;
    int high;

    bool overlaps(const Interval& other) const {
        return low <= other.high && other.low <= high;
    }
};

struct IntervalNode {
    Interval interval;
    int maxHigh;
    IntervalNode *left = nullptr, *right = nullptr;

    IntervalNode(Interval i) : interval(i), maxHigh(i.high) {}
};

class IntervalTree {
    IntervalNode* root = nullptr;

    IntervalNode* insertHelper(IntervalNode* node, Interval i) {
        if (!node) return new IntervalNode(i);

        if (i.low < node->interval.low) {
            node->left = insertHelper(node->left, i);
        } else {
            node->right = insertHelper(node->right, i);
        }

        // Update maxHigh
        if (node->maxHigh < i.high) {
            node->maxHigh = i.high;
        }

        return node;
    }

    void findOverlapsHelper(IntervalNode* node, Interval query, std::vector<Interval>& result) const {
        if (!node) return;

        if (node->interval.overlaps(query)) {
            result.push_back(node->interval);
        }

        // If left child exists and can contain an overlap
        if (node->left && node->left->maxHigh >= query.low) {
            findOverlapsHelper(node->left, query, result);
        }

        // Also check right child if needed
        findOverlapsHelper(node->right, query, result);
    }

public:
    void insert(int low, int high) {
        root = insertHelper(root, {low, high});
    }

    std::vector<Interval> findOverlaps(int qLow, int qHigh) const {
        std::vector<Interval> result;
        findOverlapsHelper(root, {qLow, qHigh}, result);
        return result;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "       Interval Tree Overlap Search       \n";
    std::cout << "==========================================\n\n";

    IntervalTree tree;
    std::vector<Interval> intervals = {
        {15, 20}, {10, 30}, {17, 19},
        {5, 20},  {12, 15}, {30, 40}
    };

    std::cout << "Inserting Intervals:\n";
    for (const auto& it : intervals) {
        std::cout << "  [" << it.low << ", " << it.high << "]\n";
        tree.insert(it.low, it.high);
    }

    std::vector<Interval> queries = {
        {14, 16}, // Overlaps with [15, 20], [10, 30], [5, 20], [12, 15]
        {21, 23}, // Overlaps with [10, 30]
        {45, 50}  // No overlap
    };

    std::cout << "\n--- Overlap Queries (O(log N)) ---\n";
    for (const auto& q : queries) {
        std::cout << "Query Range [" << q.low << ", " << q.high << "]:\n";
        auto overlaps = tree.findOverlaps(q.low, q.high);
        if (overlaps.empty()) {
            std::cout << "  [No overlapping intervals found]\n";
        } else {
            std::cout << "  Overlaps found (" << overlaps.size() << "): ";
            for (const auto& o : overlaps) {
                std::cout << "[" << o.low << ", " << o.high << "] ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    return 0;
}
