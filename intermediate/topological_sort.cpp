/*
 * Problem Description:
 * Implement Topological Sorting for a Directed Acyclic Graph (DAG) using Depth First Search.
 * Linear ordering of its vertices such that for every directed edge uv from vertex u to vertex v,
 * u comes before v in the ordering.
 */

#include <iostream>
#include <vector>
#include <stack>

using namespace std;

class Graph {
    int V; 
    vector<vector<int>> adj; 
    void topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack);

public:
    Graph(int V); 
    void addEdge(int v, int w); 
    void topologicalSort(); 
};

Graph::Graph(int V) {
    this->V = V;
    adj.resize(V);
}

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);
}

void Graph::topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;

    for (int i : adj[v])
        if (!visited[i])
            topologicalSortUtil(i, visited, Stack);

    Stack.push(v);
}

void Graph::topologicalSort() {
    stack<int> Stack;
    vector<bool> visited(V, false);

    for (int i = 0; i < V; i++)
        if (!visited[i])
            topologicalSortUtil(i, visited, Stack);

    cout << "Topological Sort of the given graph: \n";
    while (!Stack.empty()) {
        cout << Stack.top() << " ";
        Stack.pop();
    }
    cout << "\n";
}

int main() {
    Graph g(6);
    g.addEdge(5, 2);
    g.addEdge(5, 0);
    g.addEdge(4, 0);
    g.addEdge(4, 1);
    g.addEdge(2, 3);
    g.addEdge(3, 1);

    g.topologicalSort();

    return 0;
}
