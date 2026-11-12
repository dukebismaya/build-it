#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

struct Slide {
  int x1, y1, x2, y2;
  int min_x, max_x;
  int dir;
};

int main() {
  ios_base::sync_with_stdio(false);
  cin.tie(NULL);

  int n;
  if (!(cin >> n))
    return 0;

  vector<Slide> slides(n);
  for (int i = 0; i < n; i++) {
    cin >> slides[i].x1 >> slides[i].y1 >> slides[i].x2 >> slides[i].y2;
    slides[i].min_x = min(slides[i].x1, slides[i].x2);
    slides[i].max_x = max(slides[i].x1, slides[i].x2);
    if ((slides[i].x2 > slides[i].x1 && slides[i].y2 > slides[i].y1) ||
        (slides[i].x1 > slides[i].x2 && slides[i].y1 > slides[i].y2)) {
      slides[i].dir = 1;
    } else {
      slides[i].dir = -1;
    }
  }

  int start_x, start_y, energy;
  cin >> start_x >> start_y >> energy;

  int x = start_x;
  int y = start_y;

  while (y > 0) {
    bool on_slide = false;
    int slide_dir = 0;

    for (const auto &slide : slides) {
      if (x >= slide.min_x && x <= slide.max_x) {
        int slide_y;
        if (slide.dir == 1) {
          slide_y = min(slide.y1, slide.y2) + (x - slide.min_x);
        } else {
          slide_y = max(slide.y1, slide.y2) - (x - slide.min_x);
        }

        if (slide_y == y) {
          on_slide = true;
          slide_dir = slide.dir == 1 ? -1 : 1;
          break;
        }
      }
    }

    if (!on_slide) {
      y--;
    } else {
      if (energy == 0)
        break;

      int next_x = x + slide_dir;
      int next_y = y - 1;
      bool blocked = false;

      for (const auto &slide : slides) {
        if (x >= slide.min_x && x <= slide.max_x) {
          int slide_y_at_x = (slide.dir == 1)
                                 ? min(slide.y1, slide.y2) + (x - slide.min_x)
                                 : max(slide.y1, slide.y2) - (x - slide.min_x);
          int slide_y_at_next =
              (slide.dir == 1)
                  ? min(slide.y1, slide.y2) + (next_x - slide.min_x)
                  : max(slide.y1, slide.y2) - (next_x - slide.min_x);

          if (slide_y_at_x == y && slide_y_at_next > next_y) {
            blocked = true;
          }
        }
      }

      if (blocked) {
        int cost = x * y;
        if (energy >= cost) {
          energy -= cost;
          x = next_x;
          y = next_y;
          energy--;
        } else {
          break;
        }
      } else {
        x = next_x;
        y = next_y;
        energy--;
      }
    }
  }

  cout << x << " " << y << "\n";
  return 0;
}