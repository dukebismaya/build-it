#include <iostream>
#include <iomanip>

int main() {
    double celsius, fahrenheit;
    int choice;

    std::cout << "Temperature Converter\n";
    std::cout << "1. Celsius to Fahrenheit\n";
    std::cout << "2. Fahrenheit to Celsius\n";
    std::cout << "Enter choice (1 or 2): ";

    if (std::cin >> choice) {
        if (choice == 1) {
            std::cout << "Enter temperature in Celsius: ";
            std::cin >> celsius;
            fahrenheit = (celsius * 9.0 / 5.0) + 32.0;
            std::cout << std::fixed << std::setprecision(2);
            std::cout << celsius << " °C = " << fahrenheit << " °F" << std::endl;
        } else if (choice == 2) {
            std::cout << "Enter temperature in Fahrenheit: ";
            std::cin >> fahrenheit;
            celsius = (fahrenheit - 32.0) * 5.0 / 9.0;
            std::cout << std::fixed << std::setprecision(2);
            std::cout << fahrenheit << " °F = " << celsius << " °C" << std::endl;
        } else {
            std::cout << "Invalid choice." << std::endl;
        }
    } else {
        // Demo mode
        double demoC = 100.0;
        double demoF = (demoC * 9.0 / 5.0) + 32.0;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << demoC << " °C = " << demoF << " °F" << std::endl;
    }

    return 0;
}
