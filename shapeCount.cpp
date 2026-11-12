#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace std;

bool isCovered(const vector<pair<int, int>> &merged_intervals, int start,
               int end) {
  for (const auto &p : merged_intervals) {
    if (p.first <= start && p.second >= end) {
      return true;
    }
  }
  return false;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n;
  if (!(cin >> n))
    return 0;

  map<int, vector<pair<int, int>>> horizontal;
  map<int, vector<pair<int, int>>> vertical;
  set<int> x_coords;
  set<int> y_coords;

  for (int i = 0; i < n; i++) {
    int x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;

    x_coords.insert(x1);
    x_coords.insert(x2);
    y_coords.insert(y1);
    y_coords.insert(y2);

    if (y1 == y2) {
      horizontal[y1].push_back({min(x1, x2), max(x1, x2)});
    } else if (x1 == x2) {
      vertical[x1].push_back({min(y1, y2), max(y1, y2)});
    }
  }

  for (auto &kv : horizontal) {
    auto &intervals = kv.second;
    sort(intervals.begin(), intervals.end());
    vector<pair<int, int>> merged;
    for (const auto &p : intervals) {
      if (merged.empty() || merged.back().second < p.first) {
        merged.push_back(p);
      } else {
        merged.back().second = max(merged.back().second, p.second);
      }
    }
    kv.second = merged;
  }

  for (auto &kv : vertical) {
    auto &intervals = kv.second;
    sort(intervals.begin(), intervals.end());
    vector<pair<int, int>> merged;
    for (const auto &p : intervals) {
      if (merged.empty() || merged.back().second < p.first) {
        merged.push_back(p);
      } else {
        merged.back().second = max(merged.back().second, p.second);
      }
    }
    kv.second = merged;
  }

  vector<int> X(x_coords.begin(), x_coords.end());
  vector<int> Y(y_coords.begin(), y_coords.end());

  int rect_count = 0;

  for (size_t i = 0; i < X.size(); i++) {
    for (size_t j = i + 1; j < X.size(); j++) {
      for (size_t k = 0; k < Y.size(); k++) {
        for (size_t l = k + 1; l < Y.size(); l++) {
          int x1 = X[i], x2 = X[j];
          int y1 = Y[k], y2 = Y[l];

          if (isCovered(horizontal[y1], x1, x2) &&
              isCovered(horizontal[y2], x1, x2) &&
              isCovered(vertical[x1], y1, y2) &&
              isCovered(vertical[x2], y1, y2)) {
            rect_count++;
          }
        }
      }
    }
  }

  cout << rect_count << "\n";
  return 0;
}