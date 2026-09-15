#include <iostream>

int main() {
    int dividend, divisor, quotient, remainder;

    std::cout << "Enter dividend: ";
    std::cin >> dividend;
    std::cout << "Enter divisor: ";
    std::cin >> divisor;

    quotient = dividend / divisor;
    remainder = dividend % divisor;

    std::cout << "Quotient = " << quotient << "\n";
    std::cout << "Remainder = " << remainder << "\n";

    return 0;
}
