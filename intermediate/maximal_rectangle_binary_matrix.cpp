/*
 * ============================================================================
 *  Maximal Rectangle in Binary Matrix (Histogram Stack Method)
 * ============================================================================
 *
 *  Given an M x N binary matrix of 0s and 1s, find the area of the largest
 *  rectangle containing only 1s.
 *
 *  Algorithm:
 *  1) For each row, compute a histogram: height[j] = number of consecutive 1s
 *     ending at row i in column j.
 *  2) For each row's histogram, find the largest rectangle using the classic
 *     monotonic stack algorithm (largest rectangle in histogram).
 *  3) The answer is the maximum over all rows.
 *
 *  The largest rectangle in a histogram is solved in O(N) using a stack:
 *  - Maintain a stack of bar indices with non-decreasing heights.
 *  - When a shorter bar is encountered, pop and compute areas with the
 *    popped bar as the shortest.
 *
 *  Time Complexity:  O(M * N)
 *  Space Complexity: O(N) auxiliary
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <iomanip>
#include <cstdint>

namespace maximal_rect {

/*
 * Largest rectangle in histogram using monotonic stack.
 * Returns the maximum rectangular area.
 */
int64_t largestRectangleInHistogram(const std::vector<int>& heights) {
    int n = static_cast<int>(heights.size());
    std::stack<int> stk;
    int64_t maxArea = 0;

    for (int i = 0; i <= n; ++i) {
        int currHeight = (i < n) ? heights[i] : 0;

        while (!stk.empty() && heights[stk.top()] > currHeight) {
            int h = heights[stk.top()];
            stk.pop();
            int width = stk.empty() ? i : (i - stk.top() - 1);
            maxArea = std::max(maxArea, static_cast<int64_t>(h) * width);
        }
        stk.push(i);
    }

    return maxArea;
}

struct Result {
    int64_t area;
    int topRow, leftCol, bottomRow, rightCol; /* Bounding box */
};

/*
 * Find the maximal rectangle of 1s in a binary matrix.
 * Also tracks the position of the rectangle.
 */
Result maximalRectangle(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) return {0, -1, -1, -1, -1};

    int m = static_cast<int>(matrix.size());
    int n = static_cast<int>(matrix[0].size());

    std::vector<int> heights(n, 0);
    Result best = {0, -1, -1, -1, -1};

    for (int i = 0; i < m; ++i) {
        /* Update histogram heights */
        for (int j = 0; j < n; ++j) {
            heights[j] = (matrix[i][j] == 1) ? heights[j] + 1 : 0;
        }

        /* Find largest rectangle in this histogram with position tracking */
        std::stack<int> stk;
        for (int j = 0; j <= n; ++j) {
            int currH = (j < n) ? heights[j] : 0;
            while (!stk.empty() && heights[stk.top()] > currH) {
                int h = heights[stk.top()];
                stk.pop();
                int width = stk.empty() ? j : (j - stk.top() - 1);
                int64_t area = static_cast<int64_t>(h) * width;

                if (area > best.area) {
                    best.area = area;
                    best.bottomRow = i;
                    best.topRow = i - h + 1;
                    best.leftCol = stk.empty() ? 0 : stk.top() + 1;
                    best.rightCol = j - 1;
                }
            }
            stk.push(j);
        }
    }

    return best;
}

/* Brute force O(M^2 * N^2) for verification */
int64_t bruteForce(const std::vector<std::vector<int>>& matrix) {
    if (matrix.empty()) return 0;
    int m = static_cast<int>(matrix.size());
    int n = static_cast<int>(matrix[0].size());
    int64_t best = 0;

    for (int r1 = 0; r1 < m; ++r1) {
        for (int c1 = 0; c1 < n; ++c1) {
            for (int r2 = r1; r2 < m; ++r2) {
                for (int c2 = c1; c2 < n; ++c2) {
                    bool allOnes = true;
                    for (int r = r1; r <= r2 && allOnes; ++r) {
                        for (int c = c1; c <= c2 && allOnes; ++c) {
                            if (matrix[r][c] != 1) allOnes = false;
                        }
                    }
                    if (allOnes) {
                        int64_t area = static_cast<int64_t>(r2 - r1 + 1) * (c2 - c1 + 1);
                        best = std::max(best, area);
                    }
                }
            }
        }
    }
    return best;
}

void printMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        std::cout << "    ";
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

} // namespace maximal_rect

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Maximal Rectangle in Binary Matrix (Stack Method)\n";
    std::cout << "========================================================\n\n";

    /* Test 1: Standard example */
    std::vector<std::vector<int>> mat1 = {
        {1, 0, 1, 0, 0},
        {1, 0, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 0, 0, 1, 0}
    };

    std::cout << "Matrix 1 (4x5):\n";
    maximal_rect::printMatrix(mat1);

    auto res1 = maximal_rect::maximalRectangle(mat1);
    int64_t brute1 = maximal_rect::bruteForce(mat1);

    std::cout << "  Maximal rectangle area: " << res1.area << "\n";
    std::cout << "  Position: rows [" << res1.topRow << ", " << res1.bottomRow
              << "], cols [" << res1.leftCol << ", " << res1.rightCol << "]\n";
    std::cout << "  Brute force: " << brute1
              << " | Match: " << (res1.area == brute1 ? "YES" : "NO") << "\n\n";

    /* Test 2: All 1s */
    std::vector<std::vector<int>> mat2 = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };

    std::cout << "Matrix 2 (3x3 all 1s):\n";
    maximal_rect::printMatrix(mat2);

    auto res2 = maximal_rect::maximalRectangle(mat2);
    int64_t brute2 = maximal_rect::bruteForce(mat2);
    std::cout << "  Maximal rectangle area: " << res2.area
              << " | Brute: " << brute2
              << " | Match: " << (res2.area == brute2 ? "YES" : "NO") << "\n\n";

    /* Test 3: Larger matrix */
    std::vector<std::vector<int>> mat3 = {
        {0, 1, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1}
    };

    std::cout << "Matrix 3 (5x6):\n";
    maximal_rect::printMatrix(mat3);

    auto res3 = maximal_rect::maximalRectangle(mat3);
    int64_t brute3 = maximal_rect::bruteForce(mat3);
    std::cout << "  Maximal rectangle area: " << res3.area << "\n";
    std::cout << "  Position: rows [" << res3.topRow << ", " << res3.bottomRow
              << "], cols [" << res3.leftCol << ", " << res3.rightCol << "]\n";
    std::cout << "  Brute force: " << brute3
              << " | Match: " << (res3.area == brute3 ? "YES" : "NO") << "\n\n";

    /* Histogram standalone test */
    std::cout << "Histogram Standalone Test:\n";
    std::vector<int> hist = {2, 1, 5, 6, 2, 3};
    std::cout << "  Heights: [";
    for (size_t i = 0; i < hist.size(); ++i) {
        std::cout << hist[i] << (i + 1 < hist.size() ? ", " : "]\n");
    }
    std::cout << "  Largest rectangle: "
              << maximal_rect::largestRectangleInHistogram(hist)
              << " (expected 10)\n";

    return 0;
}
