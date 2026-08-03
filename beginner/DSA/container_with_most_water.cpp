#include <algorithm>
#include <vector>

using namespace std;
class Solution {
public:
  int maxArea(vector<int> &height) {
    int left{}, right = height.size() - 1;
    int maxCapacity{};
    while (left < right) {
      maxCapacity = std::max(
          std::min(height[left], height[right]) * (right - left), maxCapacity);
      if (height[left] < height[right])
        left++;
      else
        right--;
    }
    return maxCapacity;
  }
};