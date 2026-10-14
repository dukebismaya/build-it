#include <iostream>
#include <string>
#include <algorithm>

// Function to convert a decimal integer to any base (2-36)
std::string decimalToBase(int num, int base) {
    if (base < 2 || base > 36) return "Invalid base";
    if (num == 0) return "0";

    bool negative = (num < 0);
    long long n = std::abs(static_cast<long long>(num));
    std::string result;
    const char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (n > 0) {
        result += digits[n % base];
        n /= base;
    }

    if (negative) result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

// Function to convert from any base (2-36) to decimal
int baseToDecimal(const std::string& num, int base) {
    int result = 0;
    int sign = 1;
    size_t start = 0;

    if (num[0] == '-') {
        sign = -1;
        start = 1;
    }

    for (size_t i = start; i < num.size(); ++i) {
        char c = std::toupper(num[i]);
        int digit = (c >= '0' && c <= '9') ? (c - '0') : (c - 'A' + 10);
        result = result * base + digit;
    }

    return result * sign;
}

int main() {
    int decimal = 255;

    std::cout << "Number Base Converter\n\n";
    std::cout << "Decimal " << decimal << " in various bases:\n";
    std::cout << "  Binary (base 2):  " << decimalToBase(decimal, 2) << std::endl;
    std::cout << "  Octal (base 8):   " << decimalToBase(decimal, 8) << std::endl;
    std::cout << "  Hex (base 16):    " << decimalToBase(decimal, 16) << std::endl;
    std::cout << "  Base 5:           " << decimalToBase(decimal, 5) << std::endl;
    std::cout << "  Base 36:          " << decimalToBase(decimal, 36) << std::endl;

    std::cout << "\nReverse conversions:\n";
    std::cout << "  Binary 11111111 -> " << baseToDecimal("11111111", 2) << std::endl;
    std::cout << "  Hex FF -> " << baseToDecimal("FF", 16) << std::endl;
    std::cout << "  Octal 377 -> " << baseToDecimal("377", 8) << std::endl;

    return 0;
}
