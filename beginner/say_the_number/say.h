#pragma once
#include <string>
#include <vector>
namespace say {
inline std::vector<std::string> ones{
    "",        "one",     "two",       "three",     "four",
    "five",    "six",     "seven",     "eight",     "nine",
    "ten",     "eleven",  "twelve",    "thirteen",  "fourteen",
    "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"};
inline std::vector<std::string> tens{"",       "",       "twenty", "thirty",
                                     "forty",  "fifty", "sixty",  "seventy",
                                     "eighty", "ninety"};
auto say_hundred(int num) -> std::string;
auto in_english(unsigned long long num) -> std::string;
} // namespace say