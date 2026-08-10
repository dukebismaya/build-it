//  Question Link
//  https://www.hackerearth.com/community/problem/algorithm/bobs-quest-d65227d1/

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <vector>

int main() {
  // Uncomment the two lines for competitive platform
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout << "Enter the number of testcases: ";
  int T{};
  if (std::cin >> T) {
    while (T--) {
      int N{}, X{}, Y{};
      /*   std::cout << "Enter the number of (array elements, (number of
         distinct " "number in the subarray), (the number must be there in the "
                      "array atleast once)): ";*/
      std::cin >> N >> X >> Y;
      std::vector<int> arr(N);
      for (size_t i = 0; i < N; ++i) {
        std::cin >> arr[i];
      }
      std::unordered_map<int, int> freqMap;
      int left{}, right{}, max_length{};
      while (right < N) {
        freqMap[arr[right]]++;
        while (freqMap.size() > X) {
          freqMap[arr[left]]--;
          if (freqMap[arr[left]] == 0)
            freqMap.erase(arr[left]);
          left++;
        }
        if (freqMap.size() == X && freqMap.count(Y))
          max_length = std::max(max_length, right - left + 1);
        right++;
      }
      std::cout << max_length << "\n";
    }
  }

  return 0;
}