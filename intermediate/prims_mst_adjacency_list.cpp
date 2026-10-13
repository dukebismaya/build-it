/*
 * Problem Description:
 * Prims' Minimum Spanning Tree (MST) Algorithm using Adjacency List + Priority Queue.
 * Given an undirected weighted connected graph G = (V, E), find a spanning tree that connects
 * all vertices with the minimum possible total edge weight.
 * Prim's algorithm grows the MST one vertex at a time by always adding the cheapest edge
 * that connects a vertex in the MST to a vertex outside the MST.
 * This implementation uses an adjacency list and a min-heap (priority queue) for efficiency.
 * Time Complexity: O(E log V) with binary heap.
 * Space Complexity: O(V + E).
 */

#include <iostream>
#include <vector>
#include <queue>
#include <climits>

struct Edge {
    int to;
    int weight;
};

struct HeapNode {
    int vertex;
    int key;
    bool operator>(const HeapNode& other) const {
        return key > other.key;
    }
};

std::pair<int, std::vector<std::pair<int, int>>> primMST(int V, const std::vector<std::vector<Edge>>& adj) {
    std::vector<int> key(V, INT_MAX);
    std::vector<int> parent(V, -1);
    std::vector<bool> inMST(V, false);

    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>> pq;

    key[0] = 0;
    pq.push({0, 0});

    int totalWeight = 0;

    while (!pq.empty()) {
        int u = pq.top().vertex;
        pq.pop();

        if (inMST[u]) continue;
        inMST[u] = true;
        totalWeight += key[u];

        for (const Edge& e : adj[u]) {
            int v = e.to;
            int w = e.weight;
            if (!inMST[v] && w < key[v]) {
                key[v] = w;
                parent[v] = u;
                pq.push({v, key[v]});
            }
        }
    }

    std::vector<std::pair<int, int>> mstEdges;
    for (int i = 1; i < V; ++i) {
        if (parent[i] != -1) {
            mstEdges.push_back({parent[i], i});
        }
    }

    return {totalWeight, mstEdges};
}

int main() {
    int V = 7;
    std::vector<std::vector<Edge>> adj(V);

    auto addEdge = [&](int u, int v, int w) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    };

    addEdge(0, 1, 7);
    addEdge(0, 3, 5);
    addEdge(1, 2, 8);
    addEdge(1, 3, 9);
    addEdge(1, 4, 7);
    addEdge(2, 4, 5);
    addEdge(3, 4, 15);
    addEdge(3, 5, 6);
    addEdge(4, 5, 8);
    addEdge(4, 6, 9);
    addEdge(5, 6, 11);

    auto [totalWeight, mstEdges] = primMST(V, adj);

    std::cout << "Prim's Minimum Spanning Tree (MST):\n\n";
    std::cout << "MST Edges:\n";
    for (const auto& [u, v] : mstEdges) {
        std::cout << "  " << u << " -- " << v << "\n";
    }
    std::cout << "\nTotal MST Weight: " << totalWeight << "\n";

    return 0;
}
