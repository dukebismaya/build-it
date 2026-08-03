#include <algorithm>
#include <vector>

int maxProfit(std::vector<int> &prices) {
  if (prices.empty())
    return 0;
  int maxProfit{}, left{}, right{1};
  while (right < prices.size()) {
    if (prices[left] < prices[right]) {
      maxProfit = std::max(maxProfit, prices[right] - prices[left]);
    } else
      left = right;
    right++;
  }
  return maxProfit;
}