/**
 * @file box_stacking_problem_dp.cpp
 * @brief Solves the 3D Box Stacking problem for maximum height using Dynamic Programming.
 *
 * Problem:
 * Given N types of 3D rectangular boxes with dimensions (length, width, height).
 * Multiple instances of each box type can be used. Any box can be rotated so that any
 * side functions as its base. A box can be placed on top of another box only if the
 * 2D base dimensions of the upper box are strictly smaller than those of the lower box
 * (upper.length < lower.length and upper.width < lower.width).
 *
 * Algorithm:
 * 1. For each box (L, W, H), generate all 3 orientations:
 *    - Base: (max(L, W), min(L, W)), Height: H
 *    - Base: (max(W, H), min(W, H)), Height: L
 *    - Base: (max(L, H), min(L, H)), Height: W
 * 2. Total orientations = 3 * N.
 * 3. Sort all orientations descending by base area (L * W).
 * 4. Apply DP similar to Longest Increasing Subsequence (LIS):
 *    - dp[i] = height of the tallest stack with box i at the top.
 *    - dp[i] = box[i].height + max(0, max_{j < i, box[i] can rest on box[j]} dp[j])
 * 5. Reconstruct the optimal stack sequence.
 *
 * Time Complexity: O(N^2) where N is the number of box types.
 * Space Complexity: O(N) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Box {
    int id;      // original box id
    int length;  // length >= width by convention
    int width;
    int height;

    int baseArea() const {
        return length * width;
    }
};

struct BoxStackResult {
    int maxHeight;
    std::vector<Box> stack; // from bottom to top
};

BoxStackResult solveBoxStacking(const std::vector<std::vector<int>>& rawBoxes) {
    std::vector<Box> rotations;
    rotations.reserve(rawBoxes.size() * 3);

    for (size_t i = 0; i < rawBoxes.size(); ++i) {
        int d1 = rawBoxes[i][0];
        int d2 = rawBoxes[i][1];
        int d3 = rawBoxes[i][2];

        // 3 orientations
        // Orientation 1: base (d1, d2), height d3
        rotations.push_back({static_cast<int>(i + 1), std::max(d1, d2), std::min(d1, d2), d3});
        // Orientation 2: base (d2, d3), height d1
        rotations.push_back({static_cast<int>(i + 1), std::max(d2, d3), std::min(d2, d3), d1});
        // Orientation 3: base (d1, d3), height d2
        rotations.push_back({static_cast<int>(i + 1), std::max(d1, d3), std::min(d1, d3), d2});
    }

    // Sort descending by base area
    std::sort(rotations.begin(), rotations.end(), [](const Box& a, const Box& b) {
        if (a.baseArea() != b.baseArea()) {
            return a.baseArea() > b.baseArea();
        }
        return a.length > b.length;
    });

    int m = static_cast<int>(rotations.size());
    std::vector<int> dp(m, 0);
    std::vector<int> parent(m, -1);

    for (int i = 0; i < m; ++i) {
        dp[i] = rotations[i].height;
    }

    int bestIdx = 0;
    int maxHeight = 0;

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < i; ++j) {
            // Check if rotation[i] can sit on top of rotation[j]
            // rotation[j] is below, rotation[i] is above
            if (rotations[i].length < rotations[j].length &&
                rotations[i].width < rotations[j].width) {
                if (dp[j] + rotations[i].height > dp[i]) {
                    dp[i] = dp[j] + rotations[i].height;
                    parent[i] = j;
                }
            }
        }
        if (dp[i] > maxHeight) {
            maxHeight = dp[i];
            bestIdx = i;
        }
    }

    // Reconstruct stack from bottom to top
    std::vector<Box> stack;
    int curr = bestIdx;
    while (curr != -1) {
        stack.push_back(rotations[curr]);
        curr = parent[curr];
    }
    std::reverse(stack.begin(), stack.end());

    return {maxHeight, stack};
}

int main() {
    std::cout << "=== 3D Box Stacking Problem (Dynamic Programming) ===\n\n";

    std::vector<std::vector<int>> boxes1 = {
        {4, 6, 7},
        {1, 2, 3},
        {4, 5, 6},
        {10, 12, 32}
    };

    std::cout << "Input Box Types (L x W x H):\n";
    for (size_t i = 0; i < boxes1.size(); ++i) {
        std::cout << "  Type " << (i + 1) << ": "
                  << boxes1[i][0] << " x " << boxes1[i][1] << " x " << boxes1[i][2] << '\n';
    }

    auto res1 = solveBoxStacking(boxes1);

    std::cout << "\nMaximum Stack Height: " << res1.maxHeight << '\n';
    std::cout << "Optimal Stack Configuration (Bottom to Top):\n";
    std::cout << "  " << std::setw(6) << "Level"
              << std::setw(10) << "BoxType"
              << std::setw(12) << "Base (L x W)"
              << std::setw(10) << "Height"
              << std::setw(12) << "Base Area" << '\n';
    std::cout << "  " << std::string(48, '-') << '\n';

    int level = 1;
    for (const auto& b : res1.stack) {
        std::cout << "  " << std::setw(6) << level++
                  << std::setw(10) << b.id
                  << std::setw(7) << b.length << " x " << std::setw(2) << b.width
                  << std::setw(10) << b.height
                  << std::setw(12) << b.baseArea() << '\n';
    }

    return 0;
}
