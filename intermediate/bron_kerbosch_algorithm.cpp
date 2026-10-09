/*
 * Problem Description:
 * Implement Bron-Kerbosch Algorithm for finding all maximal cliques.
 * Bron-Kerbosch algorithm is a recursive backtracking algorithm that finds all maximal cliques in an undirected graph.
 */

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V) {
        this->V = V;
        adj.resize(V, vector<int>(V, 0));
    }

    void addEdge(int u, int v) {
        adj[u][v] = 1;
        adj[v][u] = 1;
    }

    void bronKerbosch(set<int>& R, set<int>& P, set<int>& X) {
        if (P.empty() && X.empty()) {
            cout << "Maximal Clique found: ";
            for (int v : R) cout << v << " ";
            cout << "\n";
            return;
        }

        set<int> P_copy = P;
        for (int v : P_copy) {
            set<int> R_new = R;
            R_new.insert(v);

            set<int> P_new, X_new;
            for (int u : P) {
                if (adj[v][u]) P_new.insert(u);
            }
            for (int u : X) {
                if (adj[v][u]) X_new.insert(u);
            }

            bronKerbosch(R_new, P_new, X_new);

            P.erase(v);
            X.insert(v);
        }
    }

    void findAllMaximalCliques() {
        set<int> R, P, X;
        for (int i = 0; i < V; i++) {
            P.insert(i);
        }
        bronKerbosch(R, P, X);
    }
};

int main() {
    Graph g(6);
    g.addEdge(0, 1);
    g.addEdge(0, 4);
    g.addEdge(1, 4);
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 4);
    g.addEdge(3, 5);

    cout << "Finding all maximal cliques:\n";
    g.findAllMaximalCliques();

    return 0;
}
