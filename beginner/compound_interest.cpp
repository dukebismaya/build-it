#include <iostream>
#include <cmath>

int main() {
    double principal, rate, time, amount, compoundInterest;

    std::cout << "Enter principal amount: ";
    std::cin >> principal;

    std::cout << "Enter annual interest rate (in %): ";
    std::cin >> rate;

    std::cout << "Enter time (in years): ";
    std::cin >> time;

    amount = principal * std::pow((1 + rate / 100), time);
    compoundInterest = amount - principal;

    std::cout << "Compound Interest = " << compoundInterest << "\n";
    std::cout << "Total Amount = " << amount << "\n";

    return 0;
}
