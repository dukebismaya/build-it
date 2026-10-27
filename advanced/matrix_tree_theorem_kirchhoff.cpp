/**
 * @file matrix_tree_theorem_kirchhoff.cpp
 * @brief Computes the number of spanning trees of an undirected graph using Kirchhoff's Matrix Tree Theorem.
 *
 * Theory:
 * - Let G = (V, E) be an undirected, loopless graph with n vertices.
 * - Degree Matrix D: D[i][i] = degree of vertex i; D[i][j] = 0 for i != j.
 * - Adjacency Matrix A: A[i][j] = number of edges between vertex i and vertex j.
 * - Laplacian Matrix L = D - A:
 *     L[i][i] = deg(i)
 *     L[i][j] = -1 if (i, j) in E, else 0 (for simple graphs)
 * - Kirchhoff's Theorem states that all cofactors of L are equal, and their common value
 *   is precisely the number of spanning trees of G.
 * - By deleting any single row and column (e.g., the last row and column), we obtain
 *   an (n-1) x (n-1) submatrix L*. The determinant det(L*) equals the number of spanning trees.
 * - Determinant is evaluated using Gaussian elimination with partial pivoting in O(V^3).
 *
 * Time Complexity: O(V^3)
 * Space Complexity: O(V^2)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>

class KirchhoffSpanningTreeCounter {
private:
    int n; // number of vertices
    std::vector<std::vector<int>> adj;

    /**
     * @brief Computes the determinant of an m x m matrix using Gaussian elimination.
     * @param mat The input square matrix.
     * @return Determinant as a double.
     */
    static double computeDeterminant(std::vector<std::vector<double>> mat) {
        int m = static_cast<int>(mat.size());
        if (m == 0) return 1.0;

        double det = 1.0;
        const double EPS = 1e-9;

        for (int col = 0; col < m; ++col) {
            // Find pivot
            int pivotRow = col;
            for (int row = col + 1; row < m; ++row) {
                if (std::abs(mat[row][col]) > std::abs(mat[pivotRow][col])) {
                    pivotRow = row;
                }
            }

            if (std::abs(mat[pivotRow][col]) < EPS) {
                return 0.0; // Singular matrix, 0 spanning trees (disconnected graph)
            }

            if (pivotRow != col) {
                std::swap(mat[col], mat[pivotRow]);
                det = -det;
            }

            det *= mat[col][col];

            for (int row = col + 1; row < m; ++row) {
                double factor = mat[row][col] / mat[col][col];
                for (int c = col; c < m; ++c) {
                    mat[row][c] -= factor * mat[col][c];
                }
            }
        }

        return det;
    }

public:
    explicit KirchhoffSpanningTreeCounter(int vertices)
        : n(vertices), adj(vertices, std::vector<int>(vertices, 0)) {}

    void addEdge(int u, int v) {
        if (u >= 0 && u < n && v >= 0 && v < n && u != v) {
            adj[u][v]++;
            adj[v][u]++;
        }
    }

    /**
     * @brief Computes the number of spanning trees.
     * @return Number of spanning trees as a 64-bit integer.
     */
    long long countSpanningTrees() const {
        if (n <= 1) return 1;

        // Build Laplacian Matrix L = D - A
        std::vector<std::vector<double>> laplacian(n, std::vector<double>(n, 0.0));
        for (int i = 0; i < n; ++i) {
            int deg = 0;
            for (int j = 0; j < n; ++j) {
                if (i != j && adj[i][j] > 0) {
                    deg += adj[i][j];
                    laplacian[i][j] = -static_cast<double>(adj[i][j]);
                }
            }
            laplacian[i][i] = deg;
        }

        // Create reduced matrix of size (n-1) x (n-1) by removing vertex (n-1)
        std::vector<std::vector<double>> reduced(n - 1, std::vector<double>(n - 1, 0.0));
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - 1; ++j) {
                reduced[i][j] = laplacian[i][j];
            }
        }

        double det = computeDeterminant(reduced);
        return static_cast<long long>(std::round(std::max(0.0, det)));
    }

    void printLaplacian() const {
        std::cout << "  Laplacian Matrix (" << n << "x" << n << "):\n";
        for (int i = 0; i < n; ++i) {
            std::cout << "  ";
            for (int j = 0; j < n; ++j) {
                int val = 0;
                if (i == j) {
                    for (int k = 0; k < n; ++k) val += adj[i][k];
                } else {
                    val = -adj[i][j];
                }
                std::cout << std::setw(4) << val;
            }
            std::cout << '\n';
        }
    }
};

int main() {
    std::cout << "=== Kirchhoff's Matrix Tree Theorem (Spanning Tree Counting) ===\n\n";

    // Test 1: Complete Graph K4 (Cayley's formula: 4^(4-2) = 16)
    {
        std::cout << "1. Complete Graph K4 (Vertices = 4):\n";
        KirchhoffSpanningTreeCounter k4(4);
        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                k4.addEdge(i, j);
            }
        }
        k4.printLaplacian();
        long long trees = k4.countSpanningTrees();
        std::cout << "  Theoretical (Cayley 4^(4-2)): 16\n";
        std::cout << "  Calculated Spanning Trees:   " << trees << "\n\n";
    }

    // Test 2: Cycle Graph C5 (Should have 5 spanning trees)
    {
        std::cout << "2. Cycle Graph C5 (Vertices = 5):\n";
        KirchhoffSpanningTreeCounter c5(5);
        for (int i = 0; i < 5; ++i) {
            c5.addEdge(i, (i + 1) % 5);
        }
        c5.printLaplacian();
        long long trees = c5.countSpanningTrees();
        std::cout << "  Theoretical (C_n has n trees): 5\n";
        std::cout << "  Calculated Spanning Trees:     " << trees << "\n\n";
    }

    // Test 3: Complete Bipartite Graph K(2, 3) (Formula: m^(n-1) * n^(m-1) = 2^2 * 3^1 = 12)
    {
        std::cout << "3. Complete Bipartite Graph K(2, 3):\n";
        KirchhoffSpanningTreeCounter k23(5);
        // Part A: {0, 1}, Part B: {2, 3, 4}
        for (int u = 0; u < 2; ++u) {
            for (int v = 2; v < 5; ++v) {
                k23.addEdge(u, v);
            }
        }
        k23.printLaplacian();
        long long trees = k23.countSpanningTrees();
        std::cout << "  Theoretical (2^2 * 3^1):   12\n";
        std::cout << "  Calculated Spanning Trees: " << trees << "\n\n";
    }

    // Test 4: Disconnected Graph (Should have 0 spanning trees)
    {
        std::cout << "4. Disconnected Graph (2 components: {0,1} and {2,3}):\n";
        KirchhoffSpanningTreeCounter disc(4);
        disc.addEdge(0, 1);
        disc.addEdge(2, 3);
        long long trees = disc.countSpanningTrees();
        std::cout << "  Theoretical:               0\n";
        std::cout << "  Calculated Spanning Trees: " << trees << '\n';
    }

    return 0;
}
