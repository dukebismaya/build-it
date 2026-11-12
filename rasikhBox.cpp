#include <iostream>
#include <string>
#include <vector>


using namespace std;

void applyGravity(vector<vector<char>> &box) {
  if (box.empty())
    return;
  int R = box.size();
  int C = box[0].size();

  for (int c = 0; c < C; ++c) {
    int stones = 0;
    for (int r = 0; r < R; ++r) {
      if (box[r][c] == '*') {
        stones++;
        box[r][c] = '.';
      }
    }
    for (int r = R - 1; r >= R - stones; --r) {
      box[r][c] = '*';
    }
  }
}

vector<vector<char>> rotateRight(const vector<vector<char>> &box) {
  int R = box.size();
  int C = box[0].size();
  vector<vector<char>> new_box(C, vector<char>(R, '.'));

  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      new_box[c][R - 1 - r] = box[r][c];
    }
  }
  return new_box;
}

vector<vector<char>> rotateLeft(const vector<vector<char>> &box) {
  int R = box.size();
  int C = box[0].size();
  vector<vector<char>> new_box(C, vector<char>(R, '.'));

  for (int r = 0; r < R; ++r) {
    for (int c = 0; c < C; ++c) {
      new_box[C - 1 - c][r] = box[r][c];
    }
  }
  return new_box;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int M, N;
  if (!(cin >> M >> N))
    return 0;

  vector<vector<char>> box(M, vector<char>(N));
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      char ch;
      cin >> ch;
      if (ch == '*') {
        box[i][j] = '*';
      } else {
        box[i][j] = '.';
      }
    }
  }

  int K;
  if (!(cin >> K))
    return 0;

  vector<string> instructions(K);
  for (int i = 0; i < K; ++i) {
    cin >> instructions[i];
  }

  applyGravity(box);

  for (int i = 0; i < K; ++i) {
    if (instructions[i] == "right") {
      box = rotateRight(box);
    } else if (instructions[i] == "left") {
      box = rotateLeft(box);
    }
    applyGravity(box);
  }

  for (int i = 0; i < box.size(); ++i) {
    for (int j = 0; j < box[0].size(); ++j) {
      cout << box[i][j] << (j == box[0].size() - 1 ? "" : " ");
    }
    cout << "\n";
  }

  return 0;
}