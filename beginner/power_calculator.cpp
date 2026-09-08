#include <iostream>

int main() {
    double base;
    int exponent;
    double result = 1.0;

    std::cout << "Enter base: ";
    std::cin >> base;
    
    std::cout << "Enter exponent: ";
    std::cin >> exponent;

    int posExponent = (exponent < 0) ? -exponent : exponent;

    for (int i = 0; i < posExponent; ++i) {
        result *= base;
    }

    if (exponent < 0) {
        result = 1.0 / result;
    }

    std::cout << base << "^" << exponent << " = " << result << "\n";

    return 0;
}
