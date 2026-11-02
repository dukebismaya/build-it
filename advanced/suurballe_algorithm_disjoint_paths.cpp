/**
 * @file suurballe_algorithm_disjoint_paths.cpp
 * @brief Computes two edge-disjoint paths from source to target with minimum total weight via Suurballe's Algorithm.
 *
 * Theory:
 * Published by John W. Suurballe in 1974, this algorithm finds two edge-disjoint paths
 * connecting source s to target t in a directed non-negative weighted graph such that
 * the sum of weights of the two paths is minimized.
 *
 * Algorithm Outline:
 * 1. Find shortest path P1 from s to t using Dijkstra's algorithm. Record distances dist1[u].
 * 2. Transform graph edge weights using node potentials to ensure non-negative reduced costs:
 *      w'(u, v) = w(u, v) - dist1[v] + dist1[u] >= 0
 * 3. Reverse the edges along P1 with reduced weight 0 in the residual graph.
 * 4. Find the shortest path P2 from s to t on the modified graph using a second Dijkstra run.
 * 5. Interlacing: Remove mutually canceling reversed edges between P1 and P2, then decompose
 *    the remaining edges into two edge-disjoint paths from s to t.
 *
 * Time Complexity: O(E * log(V)) (dominated by two Dijkstra runs)
 * Space Complexity: O(V + E)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>

struct Edge {
    int to;
    double weight;
    int id; // unique edge id
};

class SuurballeRouter {
public:
    static constexpr double INF = std::numeric_limits<double>::infinity();

    struct PathResult {
        bool success;
        double totalWeight;
        std::vector<int> path1;
        std::vector<int> path2;
    };

private:
    int n;
    std::vector<std::vector<Edge>> adj;
    std::vector<std::pair<int, int>> originalEdges;
    std::vector<double> originalWeights;

    std::pair<std::vector<double>, std::vector<int>> dijkstra(
        int src, const std::vector<std::vector<Edge>>& graph) const {
        std::vector<double> dist(n, INF);
        std::vector<int> parent(n, -1);
        std::priority_queue<std::pair<double, int>,
                            std::vector<std::pair<double, int>>,
                            std::greater<>> pq;

        dist[src] = 0;
        pq.push({0.0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;

            for (const auto& edge : graph[u]) {
                if (dist[u] + edge.weight < dist[edge.to]) {
                    dist[edge.to] = dist[u] + edge.weight;
                    parent[edge.to] = u;
                    pq.push({dist[edge.to], edge.to});
                }
            }
        }

        return {dist, parent};
    }

public:
    explicit SuurballeRouter(int vertices) : n(vertices), adj(vertices) {}

    void addEdge(int u, int v, double weight) {
        int id = static_cast<int>(originalEdges.size());
        originalEdges.push_back({u, v});
        originalWeights.push_back(weight);
        adj[u].push_back({v, weight, id});
    }

    PathResult findTwoDisjointPaths(int s, int t) {
        // Step 1: First Dijkstra to find P1
        auto [dist1, parent1] = dijkstra(s, adj);
        if (dist1[t] == INF) {
            return {false, 0.0, {}, {}}; // t unreachable
        }

        // Reconstruct P1
        std::vector<std::pair<int, int>> p1Edges;
        int curr = t;
        while (curr != s) {
            int p = parent1[curr];
            p1Edges.push_back({p, curr});
            curr = p;
        }
        std::reverse(p1Edges.begin(), p1Edges.end());

        // Step 2 & 3: Build modified residual graph with reduced costs
        // w'(u, v) = w(u, v) - dist1[v] + dist1[u]
        std::vector<std::vector<Edge>> resGraph(n);

        // Track edges of P1
        std::vector<bool> isP1(originalEdges.size(), false);
        for (const auto& edge : p1Edges) {
            for (size_t id = 0; id < originalEdges.size(); ++id) {
                if (originalEdges[id].first == edge.first && originalEdges[id].second == edge.second) {
                    isP1[id] = true;
                    break;
                }
            }
        }

        for (size_t id = 0; id < originalEdges.size(); ++id) {
            int u = originalEdges[id].first;
            int v = originalEdges[id].second;
            double w = originalWeights[id];

            if (isP1[id]) {
                // Reverse edge on P1 with weight 0
                resGraph[v].push_back({u, 0.0, static_cast<int>(id)});
            } else {
                if (dist1[u] != INF && dist1[v] != INF) {
                    double reducedWeight = w - dist1[v] + dist1[u];
                    if (reducedWeight < 0) reducedWeight = 0; // numerical safety
                    resGraph[u].push_back({v, reducedWeight, static_cast<int>(id)});
                }
            }
        }

        // Step 4: Second Dijkstra to find P2
        auto [dist2, parent2] = dijkstra(s, resGraph);
        if (dist2[t] == INF) {
            return {false, 0.0, {}, {}}; // No second edge-disjoint path
        }

        // Reconstruct P2 in residual graph
        std::vector<std::pair<int, int>> p2Edges;
        curr = t;
        while (curr != s) {
            int p = parent2[curr];
            p2Edges.push_back({p, curr});
            curr = p;
        }
        std::reverse(p2Edges.begin(), p2Edges.end());

        // Step 5: Interlace and remove antiparallel canceling edges
        std::vector<std::vector<int>> finalAdj(n);

        // Add P1 edges
        for (const auto& edge : p1Edges) {
            finalAdj[edge.first].push_back(edge.second);
        }

        // Add P2 edges or cancel
        for (const auto& edge : p2Edges) {
            // Check if (edge.second, edge.first) is in P1
            auto it = std::find(finalAdj[edge.second].begin(), finalAdj[edge.second].end(), edge.first);
            if (it != finalAdj[edge.second].end()) {
                // Cancels with reverse edge
                finalAdj[edge.second].erase(it);
            } else {
                finalAdj[edge.first].push_back(edge.second);
            }
        }

        // Extract two disjoint paths from s to t
        auto extractPath = [&](int start, int target) {
            std::vector<int> path = {start};
            int curV = start;
            while (curV != target && !finalAdj[curV].empty()) {
                int nextV = finalAdj[curV].back();
                finalAdj[curV].pop_back();
                path.push_back(nextV);
                curV = nextV;
            }
            return path;
        };

        auto finalPath1 = extractPath(s, t);
        auto finalPath2 = extractPath(s, t);

        // Compute total original weight
        auto computePathWeight = [&](const std::vector<int>& path) {
            double total = 0.0;
            for (size_t i = 0; i + 1 < path.size(); ++i) {
                int u = path[i];
                int v = path[i + 1];
                for (size_t id = 0; id < originalEdges.size(); ++id) {
                    if (originalEdges[id].first == u && originalEdges[id].second == v) {
                        total += originalWeights[id];
                        break;
                    }
                }
            }
            return total;
        };

        double totalCost = computePathWeight(finalPath1) + computePathWeight(finalPath2);

        return {true, totalCost, finalPath1, finalPath2};
    }
};

void printPath(const std::string& label, const std::vector<int>& path) {
    std::cout << label << ": ";
    for (size_t i = 0; i < path.size(); ++i) {
        std::cout << path[i];
        if (i + 1 < path.size()) std::cout << " -> ";
    }
    std::cout << '\n';
}

int main() {
    std::cout << "=== Suurballe's Algorithm (Minimum Total Weight Edge-Disjoint Paths) ===\n\n";

    // Classic diamond graph with cross edge:
    // Vertices: 0 (Source), 1, 2, 3, 4, 5 (Target)
    // 0 -> 1 (cost 1), 0 -> 2 (cost 2)
    // 1 -> 3 (cost 1), 2 -> 3 (cost 1), 2 -> 4 (cost 2)
    // 1 -> 4 (cost 2)
    // 3 -> 5 (cost 1), 4 -> 5 (cost 1)
    SuurballeRouter router(6);
    router.addEdge(0, 1, 1.0);
    router.addEdge(0, 2, 2.0);
    router.addEdge(1, 3, 1.0);
    router.addEdge(2, 3, 1.0);
    router.addEdge(1, 4, 2.0);
    router.addEdge(2, 4, 2.0);
    router.addEdge(3, 5, 1.0);
    router.addEdge(4, 5, 1.0);

    std::cout << "Finding two edge-disjoint paths from 0 to 5:\n";
    auto res = router.findTwoDisjointPaths(0, 5);

    if (res.success) {
        std::cout << "Disjoint Paths Successfully Found!\n";
        printPath("  Disjoint Path A", res.path1);
        printPath("  Disjoint Path B", res.path2);
        std::cout << "  Combined Total Cost: " << std::fixed << std::setprecision(2) << res.totalWeight << '\n';
    } else {
        std::cout << "Failed to find two edge-disjoint paths.\n";
    }

    return 0;
}
