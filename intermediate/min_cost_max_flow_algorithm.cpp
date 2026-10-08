/*
 * Problem Description:
 * Implement Minimum Cost Maximum Flow Algorithm conceptually.
 * The algorithm finds a flow of maximum value which has the minimum cost possible.
 */

#include <iostream>
#include <vector>
#include <queue>
using namespace std;

const int INF = 1e9;

struct Edge {
    int to, capacity, cost, flow, rev;
};

class MinCostMaxFlow {
    int V;
    vector<vector<Edge>> adj;
    vector<int> dist, parent, parentEdge;

public:
    MinCostMaxFlow(int V) {
        this->V = V;
        adj.resize(V);
        dist.resize(V);
        parent.resize(V);
        parentEdge.resize(V);
    }

    void addEdge(int from, int to, int capacity, int cost) {
        adj[from].push_back({to, capacity, cost, 0, (int)adj[to].size()});
        adj[to].push_back({from, 0, -cost, 0, (int)adj[from].size() - 1});
    }

    bool bellmanFord(int s, int t) {
        fill(dist.begin(), dist.end(), INF);
        dist[s] = 0;
        vector<bool> inQueue(V, false);
        queue<int> q;
        q.push(s);
        inQueue[s] = true;

        while (!q.empty()) {
            int u = q.front();
            q.pop();
            inQueue[u] = false;

            for (int i = 0; i < adj[u].size(); i++) {
                Edge& e = adj[u][i];
                if (e.capacity - e.flow > 0 && dist[e.to] > dist[u] + e.cost) {
                    dist[e.to] = dist[u] + e.cost;
                    parent[e.to] = u;
                    parentEdge[e.to] = i;
                    if (!inQueue[e.to]) {
                        q.push(e.to);
                        inQueue[e.to] = true;
                    }
                }
            }
        }
        return dist[t] != INF;
    }

    pair<int, int> getMinCostMaxFlow(int s, int t) {
        int flow = 0;
        int cost = 0;

        while (bellmanFord(s, t)) {
            int pushFlow = INF;
            for (int u = t; u != s; u = parent[u]) {
                Edge& e = adj[parent[u]][parentEdge[u]];
                pushFlow = min(pushFlow, e.capacity - e.flow);
            }

            flow += pushFlow;
            cost += pushFlow * dist[t];

            for (int u = t; u != s; u = parent[u]) {
                Edge& e = adj[parent[u]][parentEdge[u]];
                e.flow += pushFlow;
                adj[u][e.rev].flow -= pushFlow;
            }
        }
        return {flow, cost};
    }
};

int main() {
    MinCostMaxFlow mcmf(4);
    mcmf.addEdge(0, 1, 2, 1);
    mcmf.addEdge(0, 2, 2, 2);
    mcmf.addEdge(1, 2, 1, 1);
    mcmf.addEdge(1, 3, 2, 2);
    mcmf.addEdge(2, 3, 2, 1);

    pair<int, int> result = mcmf.getMinCostMaxFlow(0, 3);
    cout << "Max Flow: " << result.first << ", Min Cost: " << result.second << "\n";

    return 0;
}
