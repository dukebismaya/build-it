/*
 * Problem Description:
 * Implement Hopcroft-Karp algorithm.
 * Hopcroft-Karp algorithm is an algorithm that takes a bipartite graph and produces a maximum cardinality matching.
 */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int INF = 1e9;

class BipartiteGraph {
    int m, n;
    vector<vector<int>> adj;
    vector<int> pairU, pairV, dist;

public:
    BipartiteGraph(int m, int n) {
        this->m = m;
        this->n = n;
        adj.resize(m + 1);
        pairU.assign(m + 1, 0);
        pairV.assign(n + 1, 0);
        dist.assign(m + 1, 0);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    bool bfs() {
        queue<int> q;
        for (int u = 1; u <= m; u++) {
            if (pairU[u] == 0) {
                dist[u] = 0;
                q.push(u);
            } else {
                dist[u] = INF;
            }
        }

        dist[0] = INF;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            if (dist[u] < dist[0]) {
                for (int v : adj[u]) {
                    if (dist[pairV[v]] == INF) {
                        dist[pairV[v]] = dist[u] + 1;
                        q.push(pairV[v]);
                    }
                }
            }
        }

        return (dist[0] != INF);
    }

    bool dfs(int u) {
        if (u != 0) {
            for (int v : adj[u]) {
                if (dist[pairV[v]] == dist[u] + 1) {
                    if (dfs(pairV[v])) {
                        pairV[v] = u;
                        pairU[u] = v;
                        return true;
                    }
                }
            }
            dist[u] = INF;
            return false;
        }
        return true;
    }

    int hopcroftKarp() {
        int result = 0;
        while (bfs()) {
            for (int u = 1; u <= m; u++) {
                if (pairU[u] == 0 && dfs(u)) {
                    result++;
                }
            }
        }
        return result;
    }
};

int main() {
    BipartiteGraph g(4, 4);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 1);
    g.addEdge(3, 2);
    g.addEdge(4, 2);
    g.addEdge(4, 4);

    cout << "Size of maximum matching is " << g.hopcroftKarp() << "\n";

    return 0;
}
