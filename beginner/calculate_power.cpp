#include <iostream>

int main() {
    int base, exponent;
    long long result = 1;

    std::cout << "Enter base and exponent respectively: ";
    std::cin >> base >> exponent;

    std::cout << base << "^" << exponent << " = ";

    while (exponent != 0) {
        result *= base;
        --exponent;
    }

    std::cout << result << "\n";

    return 0;
}
