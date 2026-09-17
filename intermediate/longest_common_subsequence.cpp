/*
 * Problem Description:
 * Implement a dynamic programming solution for the Longest Common Subsequence (LCS) problem.
 * Given two sequences, find the length of longest subsequence present in both of them.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

int lcs(std::string X, std::string Y, int m, int n) {
    std::vector<std::vector<int>> L(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0)
                L[i][j] = 0;
            else if (X[i - 1] == Y[j - 1])
                L[i][j] = L[i - 1][j - 1] + 1;
            else
                L[i][j] = std::max(L[i - 1][j], L[i][j - 1]);
        }
    }
    return L[m][n];
}

int main() {
    std::string S1 = "AGGTAB";
    std::string S2 = "GXTXAYB";
    int m = S1.size();
    int n = S2.size();
    
    std::cout << "Length of LCS is " << lcs(S1, S2, m, n) << "\n";
    
    return 0;
}
