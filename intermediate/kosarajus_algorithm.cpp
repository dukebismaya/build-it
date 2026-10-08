/*
 * Problem Description:
 * Implement Kosaraju's Algorithm for finding strongly connected components.
 * Kosaraju's algorithm is a linear time algorithm to find the strongly connected components of a directed graph.
 */

#include <iostream>
#include <vector>
#include <stack>
using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;
    vector<vector<int>> revAdj;

    void dfs(int v, vector<bool>& visited, stack<int>& Stack) {
        visited[v] = true;
        for (int u : adj[v]) {
            if (!visited[u])
                dfs(u, visited, Stack);
        }
        Stack.push(v);
    }

    void dfsRev(int v, vector<bool>& visited) {
        visited[v] = true;
        cout << v << " ";
        for (int u : revAdj[v]) {
            if (!visited[u])
                dfsRev(u, visited);
        }
    }

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V);
        revAdj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        revAdj[v].push_back(u);
    }

    void printSCCs() {
        stack<int> Stack;
        vector<bool> visited(V, false);

        for (int i = 0; i < V; i++) {
            if (!visited[i])
                dfs(i, visited, Stack);
        }

        fill(visited.begin(), visited.end(), false);

        cout << "Strongly Connected Components:\n";
        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            if (!visited[v]) {
                dfsRev(v, visited);
                cout << "\n";
            }
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

    g.printSCCs();

    return 0;
}
