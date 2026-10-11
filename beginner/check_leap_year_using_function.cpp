#include <iostream>

// Function to check if a given year is a leap year
bool isLeapYear(int year) {
    if (year % 400 == 0) {
        return true;
    } else if (year % 100 == 0) {
        return false;
    } else if (year % 4 == 0) {
        return true;
    }
    return false;
}

int main() {
    int year;
    std::cout << "Enter a year: ";
    if (std::cin >> year) {
        if (isLeapYear(year)) {
            std::cout << year << " is a leap year." << std::endl;
        } else {
            std::cout << year << " is not a leap year." << std::endl;
        }
    } else {
        // Fallback demo values if running in non-interactive environment
        int testYears[] = {1900, 2000, 2024, 2026};
        for (int y : testYears) {
            std::cout << y << (isLeapYear(y) ? " is a leap year." : " is not a leap year.") << std::endl;
        }
    }

    return 0;
}
