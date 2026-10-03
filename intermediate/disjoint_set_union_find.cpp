/*
 * Problem Description:
 * Implement Disjoint Set (Union-Find) data structure.
 * It provides operations for adding new sets, merging sets (replacing them by their union), 
 * and finding a representative member of a set.
 */

#include <iostream>
#include <vector>
using namespace std;

class DisjointSet {
    vector<int> parent, rank;

public:
    DisjointSet(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++)
            parent[i] = i;
    }

    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]); // Path compression
    }

    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i != root_j) {
            // Union by rank
            if (rank[root_i] < rank[root_j])
                parent[root_i] = root_j;
            else if (rank[root_i] > rank[root_j])
                parent[root_j] = root_i;
            else {
                parent[root_j] = root_i;
                rank[root_i]++;
            }
        }
    }
};

int main() {
    DisjointSet ds(5);
    ds.unite(0, 2);
    ds.unite(4, 2);
    ds.unite(3, 1);

    if (ds.find(4) == ds.find(0))
        cout << "Yes, 4 and 0 are in the same set." << endl;
    else
        cout << "No, they are in different sets." << endl;

    if (ds.find(1) == ds.find(0))
        cout << "Yes, 1 and 0 are in the same set." << endl;
    else
        cout << "No, 1 and 0 are in different sets." << endl;

    return 0;
}
