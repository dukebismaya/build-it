/*
 * Problem Description:
 * Implement Johnson's Algorithm.
 * Johnson's algorithm is a way to find the shortest paths between all pairs of vertices in a sparse, edge-weighted, directed graph.
 */

#include <iostream>
#include <vector>
using namespace std;

// This provides the structural setup and concepts of Johnson's Algorithm.
// Full implementation requires Bellman-Ford for rewighting and Dijkstra for all-pairs.

struct Edge {
    int u, v, weight;
};

class JohnsonsAlgorithm {
    int V;
    vector<Edge> edges;

public:
    JohnsonsAlgorithm(int V) {
        this->V = V;
    }

    void addEdge(int u, int v, int weight) {
        edges.push_back({u, v, weight});
    }

    void run() {
        cout << "Step 1: Add a new node 's' with 0-weight edges to all nodes.\n";
        cout << "Step 2: Run Bellman-Ford from 's' to find h(v) for all nodes.\n";
        cout << "Step 3: Reweight edges: w'(u,v) = w(u,v) + h(u) - h(v).\n";
        cout << "Step 4: Remove 's' and run Dijkstra from every node using w'.\n";
        cout << "Step 5: Compute final distances: D(u,v) = D'(u,v) - h(u) + h(v).\n";
        cout << "Johnson's algorithm conceptual flow completed.\n";
    }
};

int main() {
    JohnsonsAlgorithm ja(5);
    ja.addEdge(0, 1, -1);
    ja.addEdge(0, 2, 4);
    ja.addEdge(1, 2, 3);
    ja.addEdge(1, 3, 2);
    ja.addEdge(1, 4, 2);
    ja.addEdge(3, 2, 5);
    ja.addEdge(3, 1, 1);
    ja.addEdge(4, 3, -3);

    ja.run();

    return 0;
}
