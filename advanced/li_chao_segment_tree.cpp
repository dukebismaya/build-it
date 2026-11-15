/*
 * ============================================================================
 *  Li Chao Segment Tree (Line Container for Convex Hull Trick)
 * ============================================================================
 *
 *  The Li Chao Tree is a segment tree over the domain of x-coordinates that
 *  maintains a set of lines y = mx + b and answers queries of the form:
 *    "What is the maximum (or minimum) y value at query point x?"
 *
 *  Unlike the classical Convex Hull Trick which requires lines to be inserted
 *  in sorted slope order, the Li Chao Tree supports arbitrary insertion order
 *  and point queries in O(log C) time, where C is the coordinate range.
 *
 *  Operations:
 *  - addLine(m, b): Insert line y = mx + b into the structure. O(log C)
 *  - query(x): Return the maximum y = mx + b over all inserted lines. O(log C)
 *
 *  The tree uses the "segment tree with lazy dominant line" approach:
 *  Each node stores the line that dominates the midpoint of its range.
 *  When inserting, we compare the new line with the stored line at the midpoint
 *  and recurse into the appropriate child for the non-dominant half.
 *
 *  Time Complexity:  O(log C) per insertion and query
 *  Space Complexity: O(C) or O(N log C) with dynamic node creation
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <limits>
#include <iomanip>
#include <algorithm>

namespace li_chao {

constexpr int64_t NEG_INF = std::numeric_limits<int64_t>::min() / 2;

struct Line {
    int64_t m, b; /* y = m*x + b */
    int64_t eval(int64_t x) const { return m * x + b; }
};

/*
 * Li Chao Segment Tree for maximum queries.
 * Domain: [lo, hi] (inclusive integer coordinates).
 */
class LiChaoTree {
public:
    LiChaoTree(int64_t lo, int64_t hi)
        : domainLo(lo), domainHi(hi) {
        int sz = 4 * static_cast<int>(hi - lo + 2);
        tree.resize(sz, {0, NEG_INF}); /* Identity line: always returns NEG_INF */
    }

    void addLine(int64_t m, int64_t b) {
        Line newLine = {m, b};
        addLine(newLine, 1, domainLo, domainHi);
    }

    int64_t query(int64_t x) const {
        return query(x, 1, domainLo, domainHi);
    }

private:
    int64_t domainLo, domainHi;
    std::vector<Line> tree;

    void addLine(Line newLine, int node, int64_t lo, int64_t hi) {
        int64_t mid = lo + (hi - lo) / 2;

        bool leftBetter = (newLine.eval(lo) > tree[node].eval(lo));
        bool midBetter = (newLine.eval(mid) > tree[node].eval(mid));

        if (midBetter) {
            std::swap(tree[node], newLine);
        }

        if (lo == hi) return;

        /* The non-dominant line may still be better in one half */
        if (leftBetter != midBetter) {
            addLine(newLine, 2 * node, lo, mid);
        } else {
            addLine(newLine, 2 * node + 1, mid + 1, hi);
        }
    }

    int64_t query(int64_t x, int node, int64_t lo, int64_t hi) const {
        int64_t best = tree[node].eval(x);
        if (lo == hi) return best;

        int64_t mid = lo + (hi - lo) / 2;
        if (x <= mid) {
            best = std::max(best, query(x, 2 * node, lo, mid));
        } else {
            best = std::max(best, query(x, 2 * node + 1, mid + 1, hi));
        }
        return best;
    }
};

/*
 * Brute force query for verification.
 */
int64_t bruteQuery(const std::vector<Line>& lines, int64_t x) {
    int64_t best = NEG_INF;
    for (const auto& line : lines) {
        best = std::max(best, line.eval(x));
    }
    return best;
}

} // namespace li_chao

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Li Chao Segment Tree (Line Container / CHT)\n";
    std::cout << "========================================================\n\n";

    const int64_t LO = -100, HI = 100;
    li_chao::LiChaoTree tree(LO, HI);

    /* Insert several lines */
    struct LineInput { int64_t m, b; };
    std::vector<LineInput> lines = {
        {2, 1},    /* y = 2x + 1  */
        {-1, 10},  /* y = -x + 10 */
        {0, 5},    /* y = 5       */
        {3, -20},  /* y = 3x - 20 */
        {-2, 15},  /* y = -2x + 15 */
        {1, 0},    /* y = x       */
    };

    std::cout << "Inserted Lines:\n";
    std::vector<li_chao::Line> allLines;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::cout << "  Line " << i << ": y = " << lines[i].m << "x";
        if (lines[i].b >= 0) std::cout << " + " << lines[i].b;
        else std::cout << " - " << (-lines[i].b);
        std::cout << "\n";

        tree.addLine(lines[i].m, lines[i].b);
        allLines.push_back({lines[i].m, lines[i].b});
    }

    /* Query at various x values */
    std::cout << "\nMax Queries:\n";
    std::cout << std::setw(8) << "x" << std::setw(16) << "Li Chao"
              << std::setw(16) << "Brute Force" << std::setw(10) << "Match?" << "\n";
    std::cout << "----------------------------------------------------\n";

    std::vector<int64_t> queryPoints = {-100, -10, -5, 0, 1, 3, 5, 10, 20, 50, 100};
    bool allMatch = true;

    for (int64_t x : queryPoints) {
        int64_t lcResult = tree.query(x);
        int64_t bfResult = li_chao::bruteQuery(allLines, x);
        bool match = (lcResult == bfResult);
        if (!match) allMatch = false;

        std::cout << std::setw(8) << x << std::setw(16) << lcResult
                  << std::setw(16) << bfResult
                  << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "----------------------------------------------------\n";
    std::cout << "All queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    /* Dynamic insertion test: add more lines and re-query */
    std::cout << "\n--- After inserting y = 5x - 50 and y = -3x + 30 ---\n";
    tree.addLine(5, -50);
    tree.addLine(-3, 30);
    allLines.push_back({5, -50});
    allLines.push_back({-3, 30});

    std::cout << std::setw(8) << "x" << std::setw(16) << "Li Chao"
              << std::setw(16) << "Brute Force" << std::setw(10) << "Match?" << "\n";
    std::cout << "----------------------------------------------------\n";

    for (int64_t x : queryPoints) {
        int64_t lcResult = tree.query(x);
        int64_t bfResult = li_chao::bruteQuery(allLines, x);
        bool match = (lcResult == bfResult);
        if (!match) allMatch = false;

        std::cout << std::setw(8) << x << std::setw(16) << lcResult
                  << std::setw(16) << bfResult
                  << std::setw(10) << (match ? "YES" : "NO") << "\n";
    }

    std::cout << "----------------------------------------------------\n";
    std::cout << "All queries verified: " << (allMatch ? "PASSED" : "FAILED") << "\n";

    return 0;
}
