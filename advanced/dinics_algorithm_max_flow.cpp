/*
 * Problem Description:
 * Implement Dinic's Algorithm to find the maximum flow in a flow network.
 * It uses BFS to construct a level graph and DFS to find augmenting paths.
 */

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

struct Edge {
    int v;
    int flow;
    int C;
    int rev;
};

class Graph {
    int V;
    vector<int> level;
    vector<vector<Edge>> adj;
public:
    Graph(int V) {
        adj.assign(V, vector<Edge>());
        this->V = V;
        level.assign(V, 0);
    }

    void addEdge(int u, int v, int C) {
        Edge a{v, 0, C, (int)adj[v].size()};
        Edge b{u, 0, 0, (int)adj[u].size()};
        adj[u].push_back(a);
        adj[v].push_back(b);
    }

    bool BFS(int s, int t);
    int sendFlow(int s, int flow, int t, vector<int>& ptr);
    int DinicMaxflow(int s, int t);
};

bool Graph::BFS(int s, int t) {
    for (int i = 0; i < V; i++)
        level[i] = -1;

    level[s] = 0;
    queue<int> q;
    q.push(s);

    vector<Edge>::iterator i;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (i = adj[u].begin(); i != adj[u].end(); i++) {
            Edge &e = *i;
            if (level[e.v] < 0 && e.flow < e.C) {
                level[e.v] = level[u] + 1;
                q.push(e.v);
            }
        }
    }
    return level[t] < 0 ? false : true;
}

int Graph::sendFlow(int u, int flow, int t, vector<int>& ptr) {
    if (u == t)
        return flow;

    for (int &i = ptr[u]; i < adj[u].size(); i++) {
        Edge &e = adj[u][i];
        if (level[e.v] == level[u] + 1 && e.flow < e.C) {
            int curr_flow = min(flow, e.C - e.flow);
            int temp_flow = sendFlow(e.v, curr_flow, t, ptr);

            if (temp_flow > 0) {
                e.flow += temp_flow;
                adj[e.v][e.rev].flow -= temp_flow;
                return temp_flow;
            }
        }
    }
    return 0;
}

int Graph::DinicMaxflow(int s, int t) {
    if (s == t)
        return -1;
    int total = 0;

    while (BFS(s, t) == true) {
        vector<int> ptr(V, 0);
        while (int flow = sendFlow(s, INT_MAX, t, ptr)) {
            total += flow;
        }
    }
    return total;
}

int main() {
    Graph g(6);
    g.addEdge(0, 1, 16);
    g.addEdge(0, 2, 13);
    g.addEdge(1, 2, 10);
    g.addEdge(1, 3, 12);
    g.addEdge(2, 1, 4);
    g.addEdge(2, 4, 14);
    g.addEdge(3, 2, 9);
    g.addEdge(3, 5, 20);
    g.addEdge(4, 3, 7);
    g.addEdge(4, 5, 4);

    cout << "Maximum flow is " << g.DinicMaxflow(0, 5) << "\n";
    return 0;
}
