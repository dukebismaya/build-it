/*
 * Problem Description:
 * Implement Depth First Search (DFS) for a Graph starting from a given vertex.
 * The graph is represented using an adjacency list.
 */

#include <iostream>
#include <vector>
#include <map>

class Graph {
public:
    std::map<int, bool> visited;
    std::map<int, std::vector<int>> adj;

    void addEdge(int v, int w);
    void DFS(int v);
};

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);
}

void Graph::DFS(int v) {
    visited[v] = true;
    std::cout << v << " ";

    std::vector<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            DFS(*i);
}

int main() {
    Graph g;
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    g.addEdge(2, 3);
    g.addEdge(3, 3);

    std::cout << "Following is Depth First Traversal (starting from vertex 2): \n";
    g.DFS(2);
    std::cout << "\n";

    return 0;
}
