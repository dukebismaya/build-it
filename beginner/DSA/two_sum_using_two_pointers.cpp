#include <vector>
using namespace std;
class Solution {
public:
  vector<int> twoSum(vector<int> &numbers, int target) {
    int left{}, right = numbers.size() - 1;
    while (left < right) {
      int getSum = numbers[left] + numbers[right];
      if (getSum == target) {
        return {left + 1, right + 1};
      } else if (getSum < target) {
        left++;
      } else {
        right--;
      }
    }
    return {};
  }
};