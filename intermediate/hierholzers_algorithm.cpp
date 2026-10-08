/*
 * Problem Description:
 * Implement Hierholzer's algorithm.
 * Hierholzer's algorithm is an algorithm for finding an Eulerian circuit in a connected directed graph.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
using namespace std;

class Graph {
    int V;
    unordered_map<int, vector<int>> adj;

public:
    Graph(int V) {
        this->V = V;
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
    }

    void printEulerCircuit() {
        if (adj.empty()) return;
        
        unordered_map<int, int> edge_count;
        for (int i = 0; i < V; i++) {
            edge_count[i] = adj[i].size();
        }

        int curr_path = 0; // Assuming we start from 0 and it has outgoing edges
        
        stack<int> curr_path_stack;
        vector<int> circuit;

        curr_path_stack.push(curr_path);

        while (!curr_path_stack.empty()) {
            if (edge_count[curr_path]) {
                curr_path_stack.push(curr_path);
                int next_node = adj[curr_path].back();
                edge_count[curr_path]--;
                adj[curr_path].pop_back();
                curr_path = next_node;
            } else {
                circuit.push_back(curr_path);
                curr_path = curr_path_stack.top();
                curr_path_stack.pop();
            }
        }

        cout << "Eulerian Circuit:\n";
        for (int i = circuit.size() - 1; i >= 0; i--) {
            cout << circuit[i] << " ";
        }
        cout << "\n";
    }
};

int main() {
    Graph g(3);
    g.addEdge(0, 1);
    g.addEdge(1, 2);
    g.addEdge(2, 0);

    g.printEulerCircuit();

    return 0;
}
