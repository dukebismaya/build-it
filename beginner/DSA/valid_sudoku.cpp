#include <vector>

using namespace std;

class Solution {
public:
  bool isValidSudoku(vector<vector<char>> &board) {
    if (board.size() != 9 || board[0].size() != 9)
      return false;
    bool row_seen[9][9]{};
    bool col_seen[9][9]{};
    bool box_seen[9][9]{};
    for (int r = 0; r < 9; ++r) {
      for (int c = 0; c < 9; ++c) {
        if (board[r][c] == '.')
          continue;
        int digit = board[r][c] - '1';
        if (digit < 0 || digit >= 9)
          return false;
        int box_idx = (r / 3) * 3 + (c / 3);
        if (row_seen[r][digit] || col_seen[c][digit] ||
            box_seen[box_idx][digit])
          return false;
        row_seen[r][digit] = true;
        col_seen[c][digit] = true;
        box_seen[box_idx][digit] = true;
      }
    }
    return true;
  }
};