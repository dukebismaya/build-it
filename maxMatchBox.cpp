#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>


using namespace std;

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n;
  if (!(cin >> n))
    return 0;

  vector<pair<double, double>> pts(n);
  for (int i = 0; i < n; i++) {
    cin >> pts[i].first >> pts[i].second;
  }

  double area = 0.0;
  double perimeter = 0.0;
  double min_len = 1e9;

  for (int i = 0; i < n; i++) {
    int next_i = (i + 1) % n;

    area += (pts[i].first * pts[next_i].second) -
            (pts[next_i].first * pts[i].second);

    double dx = abs(pts[i].first - pts[next_i].first);
    double dy = abs(pts[i].second - pts[next_i].second);
    double len = dx + dy;

    perimeter += len;
    if (len < min_len) {
      min_len = len;
    }
  }

  area = abs(area) / 2.0;

  int max_h_units = floor(((min_len - 0.1) / 2.0) * 10.0 + 1e-9);

  double max_vol = 0.0;
  for (int i = 1; i <= max_h_units; i++) {
    double h = i / 10.0;

    double current_area = area - (perimeter * h) + (4.0 * h * h);

    double vol = current_area * h;

    if (vol > max_vol) {
      max_vol = vol;
    }
  }

  cout << fixed << setprecision(2) << max_vol << "\n";

  return 0;
}