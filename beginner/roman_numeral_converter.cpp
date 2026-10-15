/*
 * Program Description:
 * Bidirectional Roman Numeral Converter in C++.
 * Converts Roman numerals (I, V, X, L, C, D, M) to Arabic numbers (1-3999)
 * and Arabic integers back into standard Roman numerals.
 *
 * Concepts covered:
 * - Hash map / Lookup tables
 * - Subtractive notation parsing (IV = 4, IX = 9, XL = 40, XC = 90, CD = 400, CM = 900)
 * - Input validation and edge case handling
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cctype>

// Converts an integer (1 <= num <= 3999) to Roman Numeral string
std::string intToRoman(int num) {
    if (num <= 0 || num > 3999) return "Invalid input (expected 1..3999)";

    const std::vector<std::pair<int, std::string>> romanTable = {
        {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
        {100,  "C"}, {90,  "XC"}, {50,  "L"}, {40,  "XL"},
        {10,   "X"}, {9,   "IX"}, {5,   "V"}, {4,   "IV"},
        {1,    "I"}
    };

    std::string result = "";
    for (const auto& entry : romanTable) {
        while (num >= entry.first) {
            result += entry.second;
            num -= entry.first;
        }
    }
    return result;
}

// Converts a Roman numeral string to an integer
int romanToInt(const std::string& s) {
    std::unordered_map<char, int> romanMap = {
        {'I', 1}, {'V', 5}, {'X', 10}, {'L', 50},
        {'C', 100}, {'D', 500}, {'M', 1000}
    };

    int total = 0;
    int n = s.length();

    for (int i = 0; i < n; ++i) {
        char current = std::toupper(s[i]);
        if (romanMap.find(current) == romanMap.end()) {
            return -1; // Invalid character found
        }

        int currentVal = romanMap[current];

        // If current value is smaller than next value, subtract it (e.g. IV: 5 - 1 = 4)
        if (i + 1 < n) {
            char next = std::toupper(s[i + 1]);
            if (romanMap.find(next) != romanMap.end() && currentVal < romanMap[next]) {
                total -= currentVal;
                continue;
            }
        }
        total += currentVal;
    }

    return total;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Roman Numeral Converter (Bi-dir)    \n";
    std::cout << "==========================================\n\n";

    std::vector<int> numbers = {3, 4, 9, 58, 1994, 2026, 3999};

    std::cout << "--- Integer to Roman & Back Verification ---\n";
    for (int num : numbers) {
        std::string roman = intToRoman(num);
        int reconstructed = romanToInt(roman);
        std::cout << "Integer: " << num
                  << "  -> Roman: " << roman
                  << "  -> Back: " << reconstructed
                  << (num == reconstructed ? " [PASS]" : " [FAIL]") << "\n";
    }

    std::cout << "\n--- Custom Roman String Tests ---\n";
    std::vector<std::string> romans = {"LVIII", "MCMXCIV", "MMXXVI", "CDXLIV"};
    for (const auto& r : romans) {
        std::cout << "Roman: " << r << " => " << romanToInt(r) << "\n";
    }

    return 0;
}
