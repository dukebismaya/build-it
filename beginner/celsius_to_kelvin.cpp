#include <iostream>

int main() {
    double celsius, kelvin;

    std::cout << "Enter temperature in Celsius: ";
    std::cin >> celsius;

    kelvin = celsius + 273.15;

    std::cout << "Temperature in Kelvin = " << kelvin << "\n";

    return 0;
}
