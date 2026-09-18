/*
 * Problem Description:
 * Implement a Disjoint Set Union (DSU) data structure with Union by Rank and Path Compression.
 * It is used to keep track of a set of elements partitioned into a number of disjoint (non-overlapping) subsets.
 */

#include <iostream>
#include <vector>

using namespace std;

class DSU {
    vector<int> parent;
    vector<int> rank;

public:
    DSU(int n) {
        parent.resize(n);
        rank.resize(n);
        for (int i = 0; i < n; i++) {
            parent[i] = i;
            rank[i] = 0;
        }
    }

    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]); 
    }

    void unionSet(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i != root_j) {
            if (rank[root_i] < rank[root_j]) {
                parent[root_i] = root_j;
            } else if (rank[root_i] > rank[root_j]) {
                parent[root_j] = root_i;
            } else {
                parent[root_j] = root_i;
                rank[root_i]++;
            }
        }
    }
    
    bool isSameSet(int i, int j) {
        return find(i) == find(j);
    }
};

int main() {
    int n = 5;
    DSU dsu(n);

    dsu.unionSet(0, 2);
    dsu.unionSet(4, 2);
    dsu.unionSet(3, 1);

    if (dsu.isSameSet(4, 0))
        cout << "4 and 0 are in the same set\n";
    else
        cout << "4 and 0 are in different sets\n";

    if (dsu.isSameSet(1, 0))
        cout << "1 and 0 are in the same set\n";
    else
        cout << "1 and 0 are in different sets\n";

    return 0;
}
