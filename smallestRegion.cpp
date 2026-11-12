#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

long long get_min_diff(long long P1, long long P2,
                       vector<pair<long long, long long>> &intervals) {
  vector<pair<long long, long long>> valid_open;

  for (auto &p : intervals) {
    long long s = max(p.first, P1);
    long long e = min(p.second, P2);
    if (s < e) {
      valid_open.push_back({s, e});
    }
  }

  sort(valid_open.begin(), valid_open.end());

  vector<pair<long long, long long>> merged;
  for (auto &p : valid_open) {
    if (merged.empty()) {
      merged.push_back(p);
    } else {
      if (p.first < merged.back().second) {
        merged.back().second = max(merged.back().second, p.second);
      } else {
        merged.push_back(p);
      }
    }
  }

  vector<pair<long long, long long>> valid_closed;
  long long current = P1;
  for (auto &p : merged) {
    if (current <= p.first) {
      valid_closed.push_back({current, p.first});
    }
    current = max(current, p.second);
  }
  if (current <= P2) {
    valid_closed.push_back({current, P2});
  }

  long long min_dist = 2e18;

  for (auto &p : valid_closed) {
    if (p.second > p.first) {
      min_dist = min(min_dist, 1LL);
    }
  }

  for (size_t i = 0; i + 1 < valid_closed.size(); ++i) {
    long long gap = valid_closed[i + 1].first - valid_closed[i].second;
    if (gap > 0) {
      min_dist = min(min_dist, gap);
    }
  }

  return min_dist;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n;
  if (!(cin >> n))
    return 0;

  vector<pair<long long, long long>> x_intervals;
  vector<pair<long long, long long>> y_intervals;

  for (int i = 0; i < n; ++i) {
    long long x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    if (x1 > x2)
      swap(x1, x2);
    if (y1 > y2)
      swap(y1, y2);

    x_intervals.push_back({x1, x2});
    y_intervals.push_back({y1, y2});
  }

  long long px1, py1, px2, py2;
  cin >> px1 >> py1 >> px2 >> py2;
  if (px1 > px2)
    swap(px1, px2);
  if (py1 > py2)
    swap(py1, py2);

  long long min_dx = get_min_diff(px1, px2, x_intervals);
  long long min_dy = get_min_diff(py1, py2, y_intervals);

  long long min_area = min_dx * min_dy;
  cout << min_area << "\n";

  return 0;
}