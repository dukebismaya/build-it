#include <algorithm>
#include <vector>

using namespace std;
class Solution {
public:
  vector<vector<int>> threeSum(vector<int> &nums) {
    std::sort(nums.begin(), nums.end());
    std::vector<vector<int>> res;
    for (int i = 0; i < (int)nums.size() - 2; ++i) {
      if (i > 0 && nums[i] == nums[i - 1])
        continue; // Skip duplicate elements
      int rest_sum = -1 * nums[i];
      int left = i + 1, right = nums.size() - 1;
      while (left < right) {
        int currSum = nums[left] + nums[right];
        if (currSum == rest_sum) {
          res.push_back({nums[i], nums[left], nums[right]});
          // Skip duplicates to avoid duplicate triplets in the result
          while (left < right && nums[left] == nums[left + 1])
            left++;
          while (left < right && nums[right] == nums[right - 1])
            right--;
          left++;
          right--;
        } else if (currSum < rest_sum) {
          left++;
        } else {
          right--;
        }
      }
    }
    return res;
  }
};