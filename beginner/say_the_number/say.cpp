#include <string>
#include <vector>
#include <stdexcept>

namespace say {

const std::vector<std::string> ones = {
    "", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
    "ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen",
    "seventeen", "eighteen", "nineteen"
};

const std::vector<std::string> tens = {
    "", "", "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"
};

std::string say_hundreds(int number) {
    std::string result;
    if (number >= 100) {
        result += ones[number / 100] + " hundred";
        number %= 100;
        if (number > 0) result += " ";
    }
    
    if (number >= 20) {
        result += tens[number / 10];
        if (number % 10 > 0) {
            result += "-" + ones[number % 10];
        }
    } else if (number > 0) {
        result += ones[number];
    }
    
    return result;
}

std::string in_english(unsigned long long number) {
    if (number == 0) return "zero";
    if (number > 999999999999ULL) {
        throw std::domain_error("Number out of range");
    }

    std::vector<std::pair<unsigned long long, std::string>> scales = {
        {1000000000ULL, "billion"},
        {1000000ULL, "million"},
        {1000ULL, "thousand"},
        {1ULL, ""}
    };

    std::string result;
    for (const auto& scale : scales) {
        if (number >= scale.first) {
            int current_val = number / scale.first;
            if (!result.empty()) result += " ";
            result += say_hundreds(current_val);
            if (!scale.second.empty()) {
                result += " " + scale.second;
            }
            number %= scale.first;
        }
    }

    return result;
}

} // namespace say
