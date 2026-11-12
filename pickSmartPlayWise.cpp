#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

using namespace std;

const double EPS = 1e-7;
const double PI = acos(-1.0);

struct Point {
  double x, y;
};

typedef pair<long long, long long> RoundPt;

RoundPt make_round(Point p) {
  return {(long long)round(p.x * 100.0), (long long)round(p.y * 100.0)};
}

Point make_point(RoundPt rp) { return {rp.first / 100.0, rp.second / 100.0}; }

double dist(Point p1, Point p2) { return hypot(p1.x - p2.x, p1.y - p2.y); }

bool get_intersection(Point p1, Point p2, Point q1, Point q2, Point &out) {
  double A1 = p2.y - p1.y;
  double B1 = p1.x - p2.x;
  double C1 = A1 * p1.x + B1 * p1.y;

  double A2 = q2.y - q1.y;
  double B2 = q1.x - q2.x;
  double C2 = A2 * q1.x + B2 * q1.y;

  double det = A1 * B2 - A2 * B1;
  if (abs(det) < EPS)
    return false;

  double x = (C1 * B2 - C2 * B1) / det;
  double y = (A1 * C2 - A2 * C1) / det;

  if (x >= min(p1.x, p2.x) - EPS && x <= max(p1.x, p2.x) + EPS &&
      y >= min(p1.y, p2.y) - EPS && y <= max(p1.y, p2.y) + EPS &&
      x >= min(q1.x, q2.x) - EPS && x <= max(q1.x, q2.x) + EPS &&
      y >= min(q1.y, q2.y) - EPS && y <= max(q1.y, q2.y) + EPS) {

    out.x = round(x * 100.0) / 100.0;
    out.y = round(y * 100.0) / 100.0;
    return true;
  }
  return false;
}

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n;
  if (!(cin >> n))
    return 0;

  vector<pair<Point, Point>> sticks(n);
  double total_stick_length = 0.0;

  for (int i = 0; i < n; i++) {
    cin >> sticks[i].first.x >> sticks[i].first.y >> sticks[i].second.x >>
        sticks[i].second.y;
    total_stick_length += dist(sticks[i].first, sticks[i].second);
  }

  map<RoundPt, int> pt_to_id;
  vector<RoundPt> id_to_pt;

  auto get_id = [&](RoundPt rp) {
    if (pt_to_id.find(rp) == pt_to_id.end()) {
      pt_to_id[rp] = id_to_pt.size();
      id_to_pt.push_back(rp);
    }
    return pt_to_id[rp];
  };

  vector<set<int>> adj_set(1000);

  for (int i = 0; i < n; i++) {
    vector<RoundPt> pts_on_segment;
    pts_on_segment.push_back(make_round(sticks[i].first));
    pts_on_segment.push_back(make_round(sticks[i].second));

    for (int j = 0; j < n; j++) {
      if (i == j)
        continue;
      Point intersect;
      if (get_intersection(sticks[i].first, sticks[i].second, sticks[j].first,
                           sticks[j].second, intersect)) {
        pts_on_segment.push_back(make_round(intersect));
      }
    }

    sort(pts_on_segment.begin(), pts_on_segment.end());
    pts_on_segment.erase(unique(pts_on_segment.begin(), pts_on_segment.end()),
                         pts_on_segment.end());

    for (size_t k = 0; k + 1 < pts_on_segment.size(); k++) {
      int u = get_id(pts_on_segment[k]);
      int v = get_id(pts_on_segment[k + 1]);
      if (adj_set.size() <= max(u, v)) {
        adj_set.resize(max(u, v) + 100);
      }
      adj_set[u].insert(v);
      adj_set[v].insert(u);
    }
  }

  int num_nodes = id_to_pt.size();
  vector<int> deg(num_nodes, 0);
  queue<int> q;

  for (int i = 0; i < num_nodes; i++) {
    deg[i] = adj_set[i].size();
    if (deg[i] <= 1) {
      q.push(i);
    }
  }

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    for (int v : adj_set[u]) {
      if (deg[v] > 1) {
        deg[v]--;
        if (deg[v] <= 1) {
          q.push(v);
        }
      }
    }
  }

  int start_node = -1;
  for (int i = 0; i < num_nodes; i++) {
    if (deg[i] > 1) {
      start_node = i;
      break;
    }
  }

  if (start_node == -1) {
    cout << "Abandoned\n";
    return 0;
  }

  vector<int> cycle;
  int curr = start_node;
  int prev = -1;

  do {
    cycle.push_back(curr);
    int next_node = -1;
    for (int v : adj_set[curr]) {
      if (deg[v] > 1 && v != prev) {
        next_node = v;
        break;
      }
    }
    prev = curr;
    curr = next_node;
  } while (curr != start_node && curr != -1);

  double kalyan_area = 0.0;
  double cycle_perimeter = 0.0;
  int c_size = cycle.size();

  for (int i = 0; i < c_size; i++) {
    Point p1 = make_point(id_to_pt[cycle[i]]);
    Point p2 = make_point(id_to_pt[cycle[(i + 1) % c_size]]);

    kalyan_area += (p1.x * p2.y - p2.x * p1.y);
    cycle_perimeter += dist(p1, p2);
  }

  kalyan_area = abs(kalyan_area) / 2.0;

  double leftover_length = max(0.0, total_stick_length - cycle_perimeter);
  double computer_area = (leftover_length * leftover_length) / (4.0 * PI);

  if (kalyan_area >= computer_area) {
    cout << "Kalyan\n";
  } else {
    cout << "Computer\n";
  }

  return 0;
}