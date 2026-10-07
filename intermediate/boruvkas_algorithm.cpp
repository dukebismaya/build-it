/*
 * Problem Description:
 * Implement Boruvka's Algorithm for Minimum Spanning Tree.
 * Boruvka's algorithm is a greedy algorithm for finding a minimum spanning tree in a graph 
 * for which all edge weights are distinct.
 */

#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    int src, dest, weight;
};

class Graph {
    int V, E;
    vector<Edge> edges;

public:
    Graph(int v, int e) {
        V = v;
        E = e;
    }

    void addEdge(int u, int v, int w) {
        edges.push_back({u, v, w});
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

    void boruvkaMST() {
        vector<int> parent(V);
        vector<int> rank(V, 0);
        vector<int> cheapest(V, -1);

        int numTrees = V;
        int MSTweight = 0;

        for (int i = 0; i < V; i++)
            parent[i] = i;

        while (numTrees > 1) {
            fill(cheapest.begin(), cheapest.end(), -1);

            for (int i = 0; i < E; i++) {
                int set1 = find(parent, edges[i].src);
                int set2 = find(parent, edges[i].dest);

                if (set1 != set2) {
                    if (cheapest[set1] == -1 || edges[cheapest[set1]].weight > edges[i].weight)
                        cheapest[set1] = i;
                    if (cheapest[set2] == -1 || edges[cheapest[set2]].weight > edges[i].weight)
                        cheapest[set2] = i;
                }
            }

            for (int i = 0; i < V; i++) {
                if (cheapest[i] != -1) {
                    int set1 = find(parent, edges[cheapest[i]].src);
                    int set2 = find(parent, edges[cheapest[i]].dest);

                    if (set1 != set2) {
                        MSTweight += edges[cheapest[i]].weight;
                        cout << "Edge " << edges[cheapest[i]].src << "-" << edges[cheapest[i]].dest << " included in MST\n";
                        Union(parent, rank, set1, set2);
                        numTrees--;
                    }
                }
            }
        }
        cout << "Weight of MST is " << MSTweight << endl;
    }
};

int main() {
    int V = 4, E = 5;
    Graph g(V, E);

    g.addEdge(0, 1, 10);
    g.addEdge(0, 2, 6);
    g.addEdge(0, 3, 5);
    g.addEdge(1, 3, 15);
    g.addEdge(2, 3, 4);

    g.boruvkaMST();

    return 0;
}
