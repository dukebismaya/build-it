// Using queue and stack
#include <iomanip>
#include <iostream>
#include <queue>
#include <stack>
#include <string>

bool is_palindrome(const std::string &s) {
  std::stack<char> stk;
  std::queue<char> q;
  for (const auto c : s) {
    if (std::isalpha(c)) {
      auto to_push = std::toupper(c);
      stk.push(to_push);
      q.push(to_push);
    }
  }

  while (!q.empty()) {
    auto c1 = q.front();
    q.pop();
    auto c2 = stk.top();
    stk.pop();
    if (c1 != c2)
      return false;
  }
  return true;
}

int main() {
  std::vector<std::string> test_strings{
      "a",
      "aa",
      "aba",
      "abba",
      "abbcbba",
      "ab",
      "abc",
      "radar",
      "bob",
      "ana",
      "avid diva",
      "Amore, Roma",
      "A Toyota's a toyota",
      "A Santa at NASA",
      "C++",
      "A man, a plan, a cat, a ham, a yak, a yam, a hat, a canal-Panama!",
      "This is a palindrome",
      "palindrome"};

  std::cout << std::boolalpha;
  std::cout << std::setw(8) << std::left << "Result" << "String" << std::endl;
  for (const auto &s : test_strings) {
    std::cout << std::setw(8) << std::left << is_palindrome(s) << s
              << std::endl;
  }
  std::cout << std::endl;
  return 0;
}