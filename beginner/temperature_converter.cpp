#include <iostream>

double celsiusToFahrenheit(double celsius) {
    return (celsius * 9.0 / 5.0) + 32.0;
}

double fahrenheitToCelsius(double fahrenheit) {
    return (fahrenheit - 32.0) * 5.0 / 9.0;
}

int main() {
    double temp;
    char unit;
    
    std::cout << "Enter temperature value: ";
    std::cin >> temp;
    std::cout << "Enter unit (C or F): ";
    std::cin >> unit;
    
    if (unit == 'C' || unit == 'c') {
        std::cout << temp << "C is " << celsiusToFahrenheit(temp) << "F\n";
    } else if (unit == 'F' || unit == 'f') {
        std::cout << temp << "F is " << fahrenheitToCelsius(temp) << "C\n";
    } else {
        std::cout << "Invalid unit!\n";
    }
    
    return 0;
}
