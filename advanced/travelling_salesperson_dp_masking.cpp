/*
 * Problem Description:
 * Solve the Travelling Salesperson Problem (TSP) using Dynamic Programming with Bitmasking.
 * It finds the shortest possible route that visits every city exactly once and returns to the starting city.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#define INF 9999999

int n = 4;
int dist[10][10] = {
    {0, 20, 42, 25},
    {20, 0, 30, 34},
    {42, 30, 0, 10},
    {25, 34, 10, 0}
};

int dp[16][4];

int tsp(int mask, int pos) {
    if (mask == ((1 << n) - 1)) {
        return dist[pos][0];
    }
    
    if (dp[mask][pos] != -1) {
        return dp[mask][pos];
    }
    
    int ans = INF;
    for (int city = 0; city < n; city++) {
        if ((mask & (1 << city)) == 0) {
            int newAns = dist[pos][city] + tsp(mask | (1 << city), city);
            ans = min(ans, newAns);
        }
    }
    
    return dp[mask][pos] = ans;
}

int main() {
    for (int i = 0; i < (1 << n); i++) {
        for (int j = 0; j < n; j++) {
            dp[i][j] = -1;
        }
    }
    
    cout << "Minimum weight hamiltonian cycle cost: " << tsp(1, 0) << endl;
    return 0;
}
