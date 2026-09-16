#include <iostream>

int main() {
    int month, year;
    std::cout << "Enter month (1-12): ";
    std::cin >> month;
    std::cout << "Enter year: ";
    std::cin >> year;

    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
            std::cout << "29 days\n";
        else
            std::cout << "28 days\n";
    } else if (month == 4 || month == 6 || month == 9 || month == 11) {
        std::cout << "30 days\n";
    } else {
        std::cout << "31 days\n";
    }

    return 0;
}
