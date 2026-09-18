/*
 * Problem Description:
 * Implement Bellman-Ford algorithm to find shortest paths from a single source vertex to all of the other vertices 
 * in a weighted digraph. Also checks for negative weight cycles.
 */

#include <iostream>
#include <vector>
#include <climits>

using namespace std;

struct Edge {
    int src, dest, weight;
};

class Graph {
public:
    int V, E;
    vector<Edge> edges;
    Graph(int V, int E) {
        this->V = V;
        this->E = E;
    }
    void addEdge(int u, int v, int w) {
        edges.push_back({u, v, w});
    }
    
    void bellmanFord(int src) {
        vector<int> dist(V, INT_MAX);
        dist[src] = 0;

        for (int i = 1; i <= V - 1; i++) {
            for (int j = 0; j < E; j++) {
                int u = edges[j].src;
                int v = edges[j].dest;
                int weight = edges[j].weight;
                if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
                    dist[v] = dist[u] + weight;
            }
        }

        for (int i = 0; i < E; i++) {
            int u = edges[i].src;
            int v = edges[i].dest;
            int weight = edges[i].weight;
            if (dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                cout << "Graph contains negative weight cycle\n";
                return;
            }
        }

        cout << "Vertex   Distance from Source\n";
        for (int i = 0; i < V; ++i)
            cout << i << "\t\t" << dist[i] << "\n";
    }
};

int main() {
    int V = 5; 
    int E = 8; 
    Graph g(V, E);

    g.addEdge(0, 1, -1);
    g.addEdge(0, 2, 4);
    g.addEdge(1, 2, 3);
    g.addEdge(1, 3, 2);
    g.addEdge(1, 4, 2);
    g.addEdge(3, 2, 5);
    g.addEdge(3, 1, 1);
    g.addEdge(4, 3, -3);

    g.bellmanFord(0);

    return 0;
}
