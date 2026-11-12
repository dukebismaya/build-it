/**
 * @file dijkstra_bidirectional_search.cpp
 * @brief Bidirectional Dijkstra shortest path search on directed graphs in C++17.
 *
 * Overview:
 *   Bidirectional Dijkstra accelerates shortest path queries between a single source S
 *   and target T by executing two concurrent Dijkstra searches:
 *     1. Forward Search: Explores outward from S along directed edges.
 *     2. Backward Search: Explores backward from T along reverse directed edges.
 *
 * Efficiency Advantage:
 *   Instead of exploring a search space of radius R (area ~ pi * R^2),
 *   two searches of radius R/2 explore two smaller balls (area ~ 2 * pi * (R/2)^2 = pi * R^2 / 2),
 *   effectively halving the number of visited states.
 *
 * Termination Condition:
 *   Let mu be the length of the best path seen so far through any bridge edge (u, v):
 *     mu = min(dist_F[u] + weight(u, v) + dist_B[v])
 *   The search can safely terminate when:
 *     min_key_F + min_key_B >= mu
 *
 * Complexity:
 *   - Time: O((V + E) log V) with significantly reduced constant factor in practice.
 *   - Space: O(V + E) for forward and reverse adjacency lists.
 */

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>

namespace graph {

struct Edge {
    int to;
    int weight;
};

class BidirectionalDijkstra {
private:
    int n;
    std::vector<std::vector<Edge>> forwardAdj;
    std::vector<std::vector<Edge>> backwardAdj;
    static constexpr int INF = std::numeric_limits<int>::max() / 2;

public:
    explicit BidirectionalDijkstra(int vertices)
        : n(vertices), forwardAdj(vertices), backwardAdj(vertices) {}

    void addEdge(int from, int to, int weight) {
        forwardAdj[from].push_back({to, weight});
        backwardAdj[to].push_back({from, weight});
    }

    struct Result {
        int distance;
        std::vector<int> path;
        int nodesExplored;
    };

    Result findShortestPath(int src, int dst) {
        if (src == dst) return {0, {src}, 1};

        std::vector<int> distF(n, INF), distB(n, INF);
        std::vector<int> parentF(n, -1), parentB(n, -1);
        std::vector<bool> settledF(n, false), settledB(n, false);

        using Pair = std::pair<int, int>; // (dist, vertex)
        std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pqF, pqB;

        distF[src] = 0;
        pqF.push({0, src});

        distB[dst] = 0;
        pqB.push({0, dst});

        int mu = INF; // Best path length found so far
        int meetNode = -1;
        int exploredCount = 0;

        while (!pqF.empty() && !pqB.empty()) {
            // Check termination condition
            if (pqF.top().first + pqB.top().first >= mu) {
                break;
            }

            // Alternate: expand the side with smaller minimum key
            if (pqF.top().first <= pqB.top().first) {
                int u = pqF.top().second;
                int d = pqF.top().first;
                pqF.pop();

                if (settledF[u] || d > distF[u]) continue;
                settledF[u] = true;
                exploredCount++;

                for (const auto& edge : forwardAdj[u]) {
                    int v = edge.to;
                    int w = edge.weight;
                    if (distF[u] + w < distF[v]) {
                        distF[v] = distF[u] + w;
                        parentF[v] = u;
                        pqF.push({distF[v], v});
                    }
                    if (settledB[v] && distF[u] + w + distB[v] < mu) {
                        mu = distF[u] + w + distB[v];
                        meetNode = v;
                    }
                }
            } else {
                int u = pqB.top().second;
                int d = pqB.top().first;
                pqB.pop();

                if (settledB[u] || d > distB[u]) continue;
                settledB[u] = true;
                exploredCount++;

                for (const auto& edge : backwardAdj[u]) {
                    int v = edge.to;
                    int w = edge.weight;
                    if (distB[u] + w < distB[v]) {
                        distB[v] = distB[u] + w;
                        parentB[v] = u;
                        pqB.push({distB[v], v});
                    }
                    if (settledF[v] && distB[u] + w + distF[v] < mu) {
                        mu = distB[u] + w + distF[v];
                        meetNode = v;
                    }
                }
            }
        }

        if (mu >= INF || meetNode == -1) {
            return {INF, {}, exploredCount};
        }

        // Reconstruct path: S -> ... -> meetNode -> ... -> T
        std::vector<int> pathForward;
        for (int curr = meetNode; curr != -1; curr = parentF[curr]) {
            pathForward.push_back(curr);
        }
        std::reverse(pathForward.begin(), pathForward.end());

        std::vector<int> pathBackward;
        for (int curr = parentB[meetNode]; curr != -1; curr = parentB[curr]) {
            pathBackward.push_back(curr);
        }

        pathForward.insert(pathForward.end(), pathBackward.begin(), pathBackward.end());
        return {mu, pathForward, exploredCount};
    }
};

} // namespace graph

int main() {
    std::cout << "========================================\n";
    std::cout << "     BIDIRECTIONAL DIJKSTRA SEARCH      \n";
    std::cout << "========================================\n\n";

    int n = 8;
    graph::BidirectionalDijkstra g(n);

    // Build directed weighted graph
    // 0 -> 1 (w=4), 0 -> 2 (w=2)
    // 1 -> 3 (w=5), 2 -> 3 (w=8), 2 -> 4 (w=10)
    // 3 -> 5 (w=6), 4 -> 5 (w=2), 4 -> 6 (w=7)
    // 5 -> 7 (w=3), 6 -> 7 (w=1)
    g.addEdge(0, 1, 4);
    g.addEdge(0, 2, 2);
    g.addEdge(1, 3, 5);
    g.addEdge(2, 3, 8);
    g.addEdge(2, 4, 10);
    g.addEdge(3, 5, 6);
    g.addEdge(4, 5, 2);
    g.addEdge(4, 6, 7);
    g.addEdge(5, 7, 3);
    g.addEdge(6, 7, 1);

    int src = 0;
    int dst = 7;
    auto res = g.findShortestPath(src, dst);

    std::cout << "Finding shortest path from Node " << src << " to Node " << dst << ":\n";
    std::cout << "  - Shortest Path Distance: " << res.distance << "\n";
    std::cout << "  - Settled Nodes Explored: " << res.nodesExplored << " / " << n << "\n";
    std::cout << "  - Optimal Path Sequence : ";
    for (size_t i = 0; i < res.path.size(); ++i) {
        std::cout << res.path[i] << (i + 1 == res.path.size() ? "" : " -> ");
    }
    std::cout << "\n\nProgram finished successfully.\n";
    return 0;
}
