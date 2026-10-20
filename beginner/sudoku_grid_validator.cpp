/*
 * Program Description:
 * Sudoku Board Validator in C++.
 * Determines if a 9x9 Sudoku board configuration is valid.
 *
 * Rules checked:
 * 1. Each row must contain the digits 1-9 without repetition.
 * 2. Each column must contain the digits 1-9 without repetition.
 * 3. Each of the nine 3x3 sub-boxes of the grid must contain the digits 1-9 without repetition.
 *
 * Note: Empty cells (marked by '.' or '0') are ignored.
 * A valid board is not necessarily solvable, only adhering to the current rules.
 *
 * Time Complexity: O(1) (fixed 9x9 board = 81 cells).
 * Space Complexity: O(1) (bitmasks / boolean arrays of size 9).
 */

#include <iostream>
#include <vector>
#include <string>

class SudokuValidator {
public:
    static bool isValidSudoku(const std::vector<std::vector<char>>& board) {
        // Bitmasks for rows, columns, and 3x3 boxes (bits 1-9)
        int rowMask[9] = {0};
        int colMask[9] = {0};
        int boxMask[9] = {0};

        for (int r = 0; r < 9; ++r) {
            for (int c = 0; c < 9; ++c) {
                char ch = board[r][c];
                if (ch == '.' || ch == '0' || ch == ' ') continue;

                if (ch < '1' || ch > '9') return false; // Invalid character

                int val = ch - '0';
                int bit = 1 << val;
                int boxIndex = (r / 3) * 3 + (c / 3);

                // Check for duplicates
                if ((rowMask[r] & bit) || (colMask[c] & bit) || (boxMask[boxIndex] & bit)) {
                    return false;
                }

                rowMask[r] |= bit;
                colMask[c] |= bit;
                boxMask[boxIndex] |= bit;
            }
        }

        return true;
    }

    static void printBoard(const std::vector<std::vector<char>>& board) {
        for (int r = 0; r < 9; ++r) {
            if (r > 0 && r % 3 == 0) {
                std::cout << "  ------+-------+------\n";
            }
            std::cout << "  ";
            for (int c = 0; c < 9; ++c) {
                if (c > 0 && c % 3 == 0) {
                    std::cout << "| ";
                }
                std::cout << board[r][c] << " ";
            }
            std::cout << "\n";
        }
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "          Sudoku Board Validator          \n";
    std::cout << "==========================================\n\n";

    // Board 1: Valid board
    std::vector<std::vector<char>> board1 = {
        {'5', '3', '.', '.', '7', '.', '.', '.', '.'},
        {'6', '.', '.', '1', '9', '5', '.', '.', '.'},
        {'.', '9', '8', '.', '.', '.', '.', '6', '.'},
        {'8', '.', '.', '.', '6', '.', '.', '.', '3'},
        {'4', '.', '.', '8', '.', '3', '.', '.', '1'},
        {'7', '.', '.', '.', '2', '.', '.', '.', '6'},
        {'.', '6', '.', '.', '.', '.', '2', '8', '.'},
        {'.', '.', '.', '4', '1', '9', '.', '.', '5'},
        {'.', '.', '.', '.', '8', '.', '.', '7', '9'}
    };

    std::cout << "Board 1:\n";
    SudokuValidator::printBoard(board1);
    bool valid1 = SudokuValidator::isValidSudoku(board1);
    std::cout << "Validation Result: " << (valid1 ? "VALID [PASS]" : "INVALID [FAIL]") << "\n\n";

    // Board 2: Invalid board (duplicate '8' in top-left 3x3 box)
    std::vector<std::vector<char>> board2 = board1;
    board2[0][0] = '8'; // Conflict with board2[2][2] or board2[3][0]

    std::cout << "Board 2 (Tampered duplicate '8' in row 0):\n";
    bool valid2 = SudokuValidator::isValidSudoku(board2);
    std::cout << "Validation Result: " << (valid2 ? "VALID" : "INVALID [DETECTED DUPLICATE]") << "\n";

    return 0;
}
