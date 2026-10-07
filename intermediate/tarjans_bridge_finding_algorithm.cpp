/*
 * Problem Description:
 * Implement Tarjan's Bridge-finding algorithm conceptually.
 * Bridges in a connected graph are edges whose removal disconnects the graph.
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;
    vector<int> tin, low;
    int timer;
    vector<pair<int, int>> bridges;

    void dfs(int v, int p = -1) {
        tin[v] = low[v] = timer++;
        for (int to : adj[v]) {
            if (to == p) continue;
            if (tin[to] != -1) {
                low[v] = min(low[v], tin[to]);
            } else {
                dfs(to, v);
                low[v] = min(low[v], low[to]);
                if (low[to] > tin[v])
                    bridges.push_back({v, to});
            }
        }
    }

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V);
        tin.assign(V, -1);
        low.assign(V, -1);
        timer = 0;
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void findBridges() {
        for (int i = 0; i < V; ++i) {
            if (tin[i] == -1)
                dfs(i);
        }
        cout << "Bridges in the graph:\n";
        for (auto edge : bridges) {
            cout << edge.first << " - " << edge.second << "\n";
        }
    }
};

int main() {
    Graph g(5);
    g.addEdge(1, 0);
    g.addEdge(0, 2);
    g.addEdge(2, 1);
    g.addEdge(0, 3);
    g.addEdge(3, 4);

    g.findBridges();

    return 0;
}
