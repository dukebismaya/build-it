#include <iostream>
#include <map>
#include <memory>
#include <vector>

class Solution {
public:
  int maxLen(std::vector<int> &arr) {
    if (arr.empty())
      return 0;
    std::map<int, int> window;
    int left{}, max_len{};
    for (int right = 0; right < arr.size(); ++right) {
      window[arr[right]]++;
      while (window.rbegin()->first - window.begin()->first > 1) {
        window[arr[left]]--;
        if (window[arr[left]] == 0)
          window.erase(arr[left]);
        left++;
      }
      max_len = std::max(max_len, right - left + 1);
    }
    return max_len;
  }
};

int main() {
  std::vector<int> arr{5, 4, 5, 5, 6, 7, 8, 8, 8, 7, 6};
  std::unique_ptr<Solution> sol = std::make_unique<Solution>();
  std::cout << "Longest subarray with max diff as one: " << sol->maxLen(arr)
            << std::endl;
  return 0;
}