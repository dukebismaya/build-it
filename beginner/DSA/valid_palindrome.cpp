#include <cctype>
#include <string>

using namespace std;
class Solution {
public:
  bool isPalindrome(string s) {
    int n = s.length();
    int l = 0, r = n - 1;
    while (l < r) {
      while (l < r && !std::isalnum(s[l])) {
        l++;
      }
      while (l < r && !std::isalnum(s[r])) {
        r--;
      }
      if (std::tolower(s[l]) != std::tolower(s[r])) {
        return false;
      }
      l++;
      r--;
    }
    return true;
  }
};