/*
 * Problem Description:
 * Implement Karger's Algorithm to find Minimum Cut conceptually.
 * Karger's algorithm is a randomized algorithm to compute a minimum cut of a connected graph.
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

struct Edge {
    int u, v;
};

class Graph {
public:
    int V, E;
    vector<Edge> edges;

    Graph(int v, int e) {
        V = v;
        E = e;
    }

    void addEdge(int u, int v) {
        edges.push_back({u, v});
    }

    int find(vector<int>& parent, int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent, parent[i]);
    }

    void Union(vector<int>& parent, vector<int>& rank, int x, int y) {
        int xroot = find(parent, x);
        int yroot = find(parent, y);

        if (rank[xroot] < rank[yroot])
            parent[xroot] = yroot;
        else if (rank[xroot] > rank[yroot])
            parent[yroot] = xroot;
        else {
            parent[yroot] = xroot;
            rank[xroot]++;
        }
    }

    int kargerMinCut() {
        int vertices = V;
        vector<int> parent(V);
        vector<int> rank(V, 0);

        for (int i = 0; i < V; i++)
            parent[i] = i;

        while (vertices > 2) {
            int i = rand() % E;

            int subset1 = find(parent, edges[i].u);
            int subset2 = find(parent, edges[i].v);

            if (subset1 != subset2) {
                vertices--;
                Union(parent, rank, subset1, subset2);
            }
        }

        int cutedges = 0;
        for (int i = 0; i < E; i++) {
            int subset1 = find(parent, edges[i].u);
            int subset2 = find(parent, edges[i].v);
            if (subset1 != subset2)
                cutedges++;
        }

        return cutedges;
    }
};

int main() {
    srand(time(NULL));
    int V = 4, E = 5;
    Graph g(V, E);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(0, 3);
    g.addEdge(1, 3);
    g.addEdge(2, 3);

    cout << "Minimum Cut found by Karger's algorithm is " << g.kargerMinCut() << endl;

    return 0;
}
