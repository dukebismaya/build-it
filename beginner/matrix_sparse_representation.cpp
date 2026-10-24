/*
 * Program Description:
 * Sparse Matrix Representation and Operations in C++.
 * A sparse matrix is a matrix in which most of the elements are zero.
 * Storing a sparse matrix as a dense 2D array wastes significant memory.
 *
 * This implementation provides:
 * 1. Triplet Representation: vector of (row, col, value) structs for non-zero elements.
 * 2. Dense to Sparse conversion and vice versa.
 * 3. Sparse Matrix Transpose in O(NonZeros).
 * 4. Sparse Matrix Addition in O(NonZeros_A + NonZeros_B).
 * 5. Sparsity percentage and memory savings metrics.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>

struct Element {
    int row;
    int col;
    int val;

    bool operator<(const Element& other) const {
        if (row != other.row) return row < other.row;
        return col < other.col;
    }
};

class SparseMatrix {
    int rows, cols;
    std::vector<Element> elements;

public:
    SparseMatrix(int r, int c) : rows(r), cols(c) {}

    // Constructs sparse matrix from standard 2D vector
    static SparseMatrix fromDense(const std::vector<std::vector<int>>& dense) {
        int r = dense.size();
        int c = dense.empty() ? 0 : dense[0].size();
        SparseMatrix sm(r, c);

        for (int i = 0; i < r; ++i) {
            for (int j = 0; j < c; ++j) {
                if (dense[i][j] != 0) {
                    sm.elements.push_back({i, j, dense[i][j]});
                }
            }
        }
        return sm;
    }

    void addElement(int r, int c, int v) {
        if (v != 0) elements.push_back({r, c, v});
    }

    // Transposes the sparse matrix
    SparseMatrix transpose() const {
        SparseMatrix t(cols, rows);
        for (const auto& elem : elements) {
            t.elements.push_back({elem.col, elem.row, elem.val});
        }
        std::sort(t.elements.begin(), t.elements.end());
        return t;
    }

    // Adds two sparse matrices
    SparseMatrix add(const SparseMatrix& other) const {
        if (rows != other.rows || cols != other.cols) {
            std::cout << "Dimension mismatch for matrix addition.\n";
            return SparseMatrix(0, 0);
        }

        SparseMatrix result(rows, cols);
        size_t i = 0, j = 0;

        while (i < elements.size() && j < other.elements.size()) {
            if (elements[i].row < other.elements[j].row ||
               (elements[i].row == other.elements[j].row && elements[i].col < other.elements[j].col)) {
                result.elements.push_back(elements[i++]);
            } else if (other.elements[j].row < elements[i].row ||
                      (other.elements[j].row == elements[i].row && other.elements[j].col < elements[i].col)) {
                result.elements.push_back(other.elements[j++]);
            } else {
                int sum = elements[i].val + other.elements[j].val;
                if (sum != 0) {
                    result.elements.push_back({elements[i].row, elements[i].col, sum});
                }
                i++;
                j++;
            }
        }

        while (i < elements.size()) result.elements.push_back(elements[i++]);
        while (j < other.elements.size()) result.elements.push_back(other.elements[j++]);

        return result;
    }

    double getSparsity() const {
        long long total = static_cast<long long>(rows) * cols;
        if (total == 0) return 0.0;
        return (1.0 - static_cast<double>(elements.size()) / total) * 100.0;
    }

    void printTriplets() const {
        std::cout << "  Row | Col | Value\n";
        std::cout << "  ----+-----+------\n";
        for (const auto& e : elements) {
            std::cout << "  " << std::setw(3) << e.row << " | "
                      << std::setw(3) << e.col << " | "
                      << std::setw(5) << e.val << "\n";
        }
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "       Sparse Matrix Representation       \n";
    std::cout << "==========================================\n\n";

    // 4x5 sparse matrix with only 4 non-zero values
    std::vector<std::vector<int>> denseA = {
        {0, 0, 3, 0, 4},
        {0, 0, 0, 0, 0},
        {0, 2, 0, 0, 0},
        {0, 0, 0, 5, 0}
    };

    SparseMatrix smA = SparseMatrix::fromDense(denseA);

    std::cout << "Sparse Matrix A (4x5, Sparsity: " << std::fixed
              << std::setprecision(1) << smA.getSparsity() << "%):\n";
    smA.printTriplets();

    std::cout << "\nTranspose of Matrix A (5x4):\n";
    SparseMatrix transA = smA.transpose();
    transA.printTriplets();

    // Sparse Matrix B
    std::vector<std::vector<int>> denseB = {
        {0, 0, 7, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 2, 0, 0, 0},
        {0, 0, 0, 0, 9}
    };
    SparseMatrix smB = SparseMatrix::fromDense(denseB);

    std::cout << "\nSparse Matrix B (4x5):\n";
    smB.printTriplets();

    std::cout << "\nSparse Addition (A + B):\n";
    SparseMatrix sum = smA.add(smB);
    sum.printTriplets();

    return 0;
}
