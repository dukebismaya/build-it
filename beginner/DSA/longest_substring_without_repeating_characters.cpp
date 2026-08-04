#include <algorithm>
#include <string>
#include <unordered_set>

using namespace std;
class Solution {
public:
  int lengthOfLongestSubstring(string s) {
    unordered_set<char> charSet;
    int left = 0;
    int max_substr_length = 0;

    for (int right = 0; right < s.length(); right++) {
      while (charSet.count(s[right])) {
        charSet.erase(s[left]);
        left++;
      }

      charSet.insert(s[right]);
      max_substr_length = max(max_substr_length, right - left + 1);
    }

    return max_substr_length;
  }
};