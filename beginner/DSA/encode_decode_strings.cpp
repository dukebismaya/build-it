#include <string>
#include <vector>
using namespace std;

class Solution {
public:
  string encode(vector<string> &strs) {
    std::string encoded_str{};
    for (const auto &str : strs) {
      encoded_str += (std::to_string(str.length()) + '#' + str);
    }
    return encoded_str;
  }

  vector<string> decode(string s) {
    std::vector<std::string> res;
    size_t i = 0;
    while (i < s.size()) {
      size_t j = i;
      while (s[j] != '#') {
        j++;
      }
      int length = std::stoi(s.substr(i, j - i));
      res.push_back(s.substr(j + 1, length));
      i = j + length + 1;
    }
    return res;
  }
};
