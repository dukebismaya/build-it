/*
 * Problem Description:
 * Implement Stoer-Wagner min-cut algorithm conceptually.
 * The Stoer-Wagner algorithm is a recursive algorithm to solve the minimum cut problem in undirected weighted graphs.
 */

#include <iostream>
#include <vector>
using namespace std;

const int INF = 1e9;

// Conceptually structured for finding a global min-cut
int stoerWagnerMinCut(vector<vector<int>>& mat) {
    int n = mat.size();
    int minCut = INF;
    vector<int> v(n);
    for (int i = 0; i < n; i++) v[i] = i;

    while (n > 1) {
        vector<int> w = mat[v[0]];
        vector<bool> added(n, false);
        added[v[0]] = true;
        int prev = v[0], last = v[0];

        for (int i = 1; i < n; i++) {
            int max_w = -1, best = -1;
            for (int j = 0; j < n; j++) {
                if (!added[v[j]] && w[v[j]] > max_w) {
                    max_w = w[v[j]];
                    best = j;
                }
            }
            if (i == n - 1) {
                if (max_w < minCut) minCut = max_w;
                // Merge the last two nodes
                for (int j = 0; j < n; j++) {
                    mat[v[prev]][v[j]] += mat[v[last]][v[j]];
                    mat[v[j]][v[prev]] = mat[v[prev]][v[j]];
                }
                v[best] = v[n - 1]; // Remove last
            } else {
                added[v[best]] = true;
                for (int j = 0; j < n; j++) {
                    if (!added[v[j]]) {
                        w[v[j]] += mat[v[best]][v[j]];
                    }
                }
                prev = last;
                last = best;
            }
        }
        n--;
    }
    return minCut;
}

int main() {
    int V = 4;
    vector<vector<int>> mat = {
        {0, 2, 0, 3},
        {2, 0, 3, 0},
        {0, 3, 0, 4},
        {3, 0, 4, 0}
    };

    cout << "Global Minimum Cut is: " << stoerWagnerMinCut(mat) << "\n";
    return 0;
}
