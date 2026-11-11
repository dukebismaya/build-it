/**
 * @file pagerank_power_iteration.cpp
 * @brief PageRank algorithm via damped Power Iteration with dangling-node handling in C++17.
 *
 * PageRank (Brin & Page, 1998) models a "random surfer" navigating a directed web graph:
 *   - With probability d (damping factor, typically 0.85), the surfer follows an
 *     outgoing link uniformly at random.
 *   - With probability (1 - d), the surfer teleports to any page uniformly at random.
 *   - Dangling nodes (pages with out-degree 0) redistribute their PageRank mass uniformly
 *     across all N pages to preserve stochastic probability conservation (sum(PR) == 1).
 *
 * Update Equation:
 *   PR_{t+1}(v) = (1 - d) / N + d * ( DanglingMass_t / N + sum_{u -> v} PR_t(u) / outDeg(u) )
 *
 * Complexity:
 *   - Each iteration takes O(V + E) time.
 *   - Converges exponentially fast at rate O(d^K) ~ O(0.85^K).
 *   - Space: O(V + E).
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <string>

namespace graph {

class PageRankSolver {
private:
    int n;
    std::vector<std::vector<int>> adj; // Outgoing edges u -> v
    std::vector<int> outDegree;

public:
    explicit PageRankSolver(int numNodes) : n(numNodes), adj(numNodes), outDegree(numNodes, 0) {}

    void addLink(int from, int to) {
        adj[from].push_back(to);
        outDegree[from]++;
    }

    struct Result {
        std::vector<double> scores;
        int iterations;
        double residualL1;
    };

    Result compute(double dampingFactor = 0.85, double tol = 1e-10, int maxIter = 500) const {
        std::vector<double> pr(n, 1.0 / static_cast<double>(n));
        std::vector<double> nextPr(n);

        int iter = 0;
        double diff = 0.0;

        for (; iter < maxIter; ++iter) {
            // Compute total rank mass trapped at dangling nodes (out-degree == 0)
            double danglingSum = 0.0;
            for (int u = 0; u < n; ++u) {
                if (outDegree[u] == 0) {
                    danglingSum += pr[u];
                }
            }

            double baseMass = (1.0 - dampingFactor) / n + dampingFactor * (danglingSum / n);
            std::fill(nextPr.begin(), nextPr.end(), baseMass);

            // Distribute rank along outgoing links
            for (int u = 0; u < n; ++u) {
                if (outDegree[u] > 0) {
                    double share = dampingFactor * (pr[u] / static_cast<double>(outDegree[u]));
                    for (int v : adj[u]) {
                        nextPr[v] += share;
                    }
                }
            }

            // Compute L1 norm of change
            diff = 0.0;
            for (int i = 0; i < n; ++i) {
                diff += std::abs(nextPr[i] - pr[i]);
            }

            pr = nextPr;
            if (diff < tol) {
                return {pr, iter + 1, diff};
            }
        }

        return {pr, iter, diff};
    }
};

} // namespace graph

/*
 * Demonstration Web Graph (6 Pages):
 *   Page 0 -> links to 1, 2
 *   Page 1 -> links to 2
 *   Page 2 -> links to 0, 3
 *   Page 3 -> links to 2, 4
 *   Page 4 -> links to 2, 5
 *   Page 5 -> (Dangling Node - no outgoing links!)
 */

int main() {
    std::cout << "========================================\n";
    std::cout << "     PAGERANK (POWER ITERATION)         \n";
    std::cout << "========================================\n\n";

    int numPages = 6;
    graph::PageRankSolver solver(numPages);

    solver.addLink(0, 1);
    solver.addLink(0, 2);
    solver.addLink(1, 2);
    solver.addLink(2, 0);
    solver.addLink(2, 3);
    solver.addLink(3, 2);
    solver.addLink(3, 4);
    solver.addLink(4, 2);
    solver.addLink(4, 5);
    // Node 5 is dangling

    auto res = solver.compute(0.85, 1e-10, 500);

    std::cout << "Converged in " << res.iterations << " iterations (L1 residual = "
              << std::scientific << res.residualL1 << ").\n\n";

    // Rank pages in descending order
    std::vector<std::pair<double, int>> ranked;
    for (int i = 0; i < numPages; ++i) {
        ranked.push_back({res.scores[i], i});
    }
    std::sort(ranked.rbegin(), ranked.rend());

    std::cout << std::left << std::setw(8)  << "Rank"
              << std::setw(12) << "Page ID"
              << std::setw(16) << "PageRank Score"
              << "Share (%)\n";
    std::cout << std::string(48, '-') << "\n";

    double totalMass = 0.0;
    for (size_t i = 0; i < ranked.size(); ++i) {
        double score = ranked[i].first;
        totalMass += score;
        std::cout << std::left << std::setw(8) << (i + 1)
                  << "Page " << std::setw(7) << ranked[i].second
                  << std::fixed << std::setprecision(6) << std::setw(16) << score
                  << std::setprecision(2) << (score * 100.0) << "%\n";
    }

    std::cout << "\nTotal Probability Mass Conserved: " << std::setprecision(10) << totalMass << "\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
