/**
 * @file strontio_number_checker.cpp
 * @brief Checker and range generator for Strontio Numbers in C++17.
 *
 * A Strontio number is a four-digit number N such that when multiplied by 2,
 * the resulting number has identical digits in both its tens and hundreds places.
 * That is, if (N * 2) = ...dd..., the tens digit equals the hundreds digit.
 * Formally, for a 4-digit number N in [1000, 9999]:
 *   let prod = N * 2;
 *   let tens = (prod / 10) % 10;
 *   let hundreds = (prod / 100) % 10;
 *   N is Strontio iff tens == hundreds.
 *
 * Examples of Strontio numbers:
 *   1386 -> 1386 * 2 = 2772 -> tens: 7, hundreds: 7 -> Strontio
 *   1221 -> 1221 * 2 = 2442 -> tens: 4, hundreds: 4 -> Strontio
 *
 * Complexity:
 *   - Check single number: O(1) time, O(1) space.
 *   - Range search in [1000, 9999]: O(R) time where R <= 9000, O(1) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

namespace math {

/**
 * @brief Checks if a given number is a valid 4-digit Strontio number.
 * @param n Integer to evaluate.
 * @return True if n is a 4-digit Strontio number, false otherwise.
 */
bool isStrontioNumber(int n) {
    if (n < 1000 || n > 9999) {
        return false;
    }
    int doubled = n * 2;
    int tens = (doubled / 10) % 10;
    int hundreds = (doubled / 100) % 10;
    return tens == hundreds;
}

/**
 * @brief Finds all Strontio numbers within a specified range [low, high].
 * @param low Starting integer of range (clamped to at least 1000).
 * @param high Ending integer of range (clamped to at most 9999).
 * @return Vector of Strontio numbers within [low, high].
 */
std::vector<int> getStrontioNumbersInRange(int low, int high) {
    std::vector<int> results;
    int start = (low < 1000) ? 1000 : low;
    int end = (high > 9999) ? 9999 : high;

    for (int i = start; i <= end; ++i) {
        if (isStrontioNumber(i)) {
            results.push_back(i);
        }
    }
    return results;
}

} // namespace math

int main() {
    std::cout << "========================================\n";
    std::cout << "       STRONTIO NUMBER CHECKER          \n";
    std::cout << "========================================\n\n";

    // Test cases
    std::vector<int> testCases = {1001, 1111, 1221, 1386, 1400, 1500, 2002, 3141, 7777, 9999};

    std::cout << "Individual Checks:\n";
    std::cout << std::left << std::setw(8) << "Number"
              << std::setw(12) << "Doubled"
              << std::setw(10) << "Hundreds"
              << std::setw(8)  << "Tens"
              << "Is Strontio?\n";
    std::cout << std::string(50, '-') << "\n";

    for (int num : testCases) {
        int doubled = num * 2;
        int hundreds = (doubled / 100) % 10;
        int tens = (doubled / 10) % 10;
        bool isStrontio = math::isStrontioNumber(num);

        std::cout << std::left << std::setw(8) << num
                  << std::setw(12) << doubled
                  << std::setw(10) << hundreds
                  << std::setw(8)  << tens
                  << (isStrontio ? "YES" : "NO") << "\n";
    }

    // Range demonstration
    int rangeLow = 1000;
    int rangeHigh = 1200;
    std::vector<int> strontios = math::getStrontioNumbersInRange(rangeLow, rangeHigh);

    std::cout << "\nStrontio numbers in range [" << rangeLow << ", " << rangeHigh << "]:\n";
    std::cout << "Total found: " << strontios.size() << "\n";
    for (size_t i = 0; i < strontios.size(); ++i) {
        std::cout << strontios[i] << (i + 1 == strontios.size() ? "" : ", ");
        if ((i + 1) % 10 == 0) std::cout << "\n";
    }
    std::cout << "\n\nProgram finished successfully.\n";
    return 0;
}
