/*
 * Problem Description:
 * Implement Breadth First Search (BFS) for a Graph starting from a given vertex.
 * The graph is represented using an adjacency list.
 */

#include <iostream>
#include <vector>
#include <list>

class Graph {
    int V; 
    std::vector<std::list<int>> adj;   
public:
    Graph(int V);
    void addEdge(int v, int w); 
    void BFS(int s);  
};

Graph::Graph(int V) {
    this->V = V;
    adj.resize(V);
}

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);
}

void Graph::BFS(int s) {
    std::vector<bool> visited(V, false);
    std::list<int> queue;

    visited[s] = true;
    queue.push_back(s);

    while (!queue.empty()) {
        s = queue.front();
        std::cout << s << " ";
        queue.pop_front();

        for (auto i = adj[s].begin(); i != adj[s].end(); ++i) {
            if (!visited[*i]) {
                visited[*i] = true;
                queue.push_back(*i);
            }
        }
    }
}

int main() {
    Graph g(4);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 0);
    g.addEdge(2, 3);
    g.addEdge(3, 3);

    std::cout << "Following is Breadth First Traversal (starting from vertex 2): \n";
    g.BFS(2);
    std::cout << "\n";

    return 0;
}
