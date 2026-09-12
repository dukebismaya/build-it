#include <iostream>

int main() {
    double principal, rate, time, interest;

    std::cout << "Enter principal amount: ";
    std::cin >> principal;

    std::cout << "Enter annual interest rate (in %): ";
    std::cin >> rate;

    std::cout << "Enter time (in years): ";
    std::cin >> time;

    interest = (principal * rate * time) / 100;

    std::cout << "Simple Interest = " << interest << "\n";

    return 0;
}
