#include "say.h"
#include <stdexcept>
#include <utility>
namespace say {
auto say_hundred(int num) -> std::string {
  std::string result{};
  if (num >= 100) {
    result += ones[num / 100] + " hundred";
    num %= 100;
    if (num > 0)
      result += " ";
  }
  if (num >= 20) {
    result += tens[num / 10];
    if (num % 10 > 0)
      result += "-" + ones[num % 10];
  } else {
    result += ones[num];
  }
  return result;
}
auto in_english(unsigned long long num) -> std::string {
  if (num == 0)
    return "zero";
  if (num > 999'999'999'999)
    throw std::domain_error("number is out of range.");
  std::string result{};
  std::vector<std::pair<unsigned long long, std::string>> scales{
      {1'000'000'000ULL, "billion"},
      {1'000'000ULL, "million"},
      {1'000ULL, "thousand"},
      {1ULL, ""}};
  for (const auto &p : scales) {
    if (num >= p.first) {
      if (!result.empty())
        result += " ";
      result += say_hundred(num / p.first);
      if (!p.second.empty())
        result += " " + p.second;
      num %= p.first;
    }
  }
  return result;
}

} // namespace say
