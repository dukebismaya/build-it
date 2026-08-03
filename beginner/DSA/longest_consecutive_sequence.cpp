#include <algorithm>
#include <unordered_set>
#include <vector>

using namespace std;

class Solution {
public:
  int longestConsecutive(vector<int> &nums) {
    if (nums.empty())
      return 0;

    unordered_set<int> num_set(nums.begin(), nums.end());
    int maxLCS = 0;

    for (int num : num_set) {
      // Only start counting if 'num' is the beginning of a sequence
      if (!num_set.count(num - 1)) {
        int current_num = num;
        int current_streak = 1;

        while (num_set.count(current_num + 1)) {
          current_num++;
          current_streak++;
        }

        maxLCS = max(maxLCS, current_streak);
      }
    }

    return maxLCS;
  }
};