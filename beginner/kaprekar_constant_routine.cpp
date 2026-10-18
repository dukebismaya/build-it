/*
 * Program Description:
 * Kaprekar's Routine (Kaprekar's Constant 6174) in C++.
 * Discovered by mathematician D. R. Kaprekar in 1949.
 *
 * Algorithm for 4-digit numbers:
 * 1. Choose any four-digit number with at least two distinct digits (e.g. 3524).
 * 2. Arrange the digits in descending and ascending order to get two four-digit numbers:
 *    Descending: 5432, Ascending: 2345.
 * 3. Subtract the smaller number from the larger number: 5432 - 2345 = 3087.
 * 4. Repeat the process with the result.
 * 5. This process always reaches 6174 in at most 7 iterations!
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>

const int KAPREKAR_CONSTANT = 6174;

// Pads an integer with leading zeros to make it 4 digits
std::string toFourDigits(int num) {
    std::string s = std::to_string(num);
    while (s.length() < 4) {
        s = "0" + s;
    }
    return s;
}

// Checks if the number has at least two distinct digits
bool hasAtLeastTwoDistinctDigits(int num) {
    std::string s = toFourDigits(num);
    return !(s[0] == s[1] && s[1] == s[2] && s[2] == s[3]);
}

// Executes Kaprekar's routine and prints each step
int kaprekarRoutine(int num) {
    if (num < 1000 || num > 9999 || !hasAtLeastTwoDistinctDigits(num)) {
        std::cout << "Invalid input: number must be 4 digits with at least two distinct digits.\n";
        return -1;
    }

    int current = num;
    int steps = 0;

    std::cout << "Starting Kaprekar's routine for " << num << ":\n";

    while (current != KAPREKAR_CONSTANT && steps < 10) {
        steps++;
        std::string s = toFourDigits(current);

        // Sort descending
        std::string desc = s;
        std::sort(desc.begin(), desc.end(), std::greater<char>());

        // Sort ascending
        std::string asc = s;
        std::sort(asc.begin(), asc.end());

        int numDesc = std::stoi(desc);
        int numAsc = std::stoi(asc);
        current = numDesc - numAsc;

        std::cout << "  Step " << steps << ": " << desc << " - " << asc
                  << " = " << std::setw(4) << current << "\n";
    }

    std::cout << "Reached 6174 in " << steps << " step(s)!\n\n";
    return steps;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Kaprekar's Routine (6174 Simulator)    \n";
    std::cout << "==========================================\n\n";

    std::vector<int> testNumbers = {3524, 1234, 9831, 2026, 8887};

    for (int num : testNumbers) {
        kaprekarRoutine(num);
    }

    return 0;
}
