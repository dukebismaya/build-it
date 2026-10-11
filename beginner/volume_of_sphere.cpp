#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    double radius;
    const double PI = 3.14159265358979323846;

    std::cout << "Enter the radius of the sphere: ";
    if (std::cin >> radius) {
        if (radius < 0) {
            std::cout << "Radius cannot be negative." << std::endl;
        } else {
            double volume = (4.0 / 3.0) * PI * std::pow(radius, 3);
            std::cout << std::fixed << std::setprecision(4);
            std::cout << "Volume of the sphere: " << volume << std::endl;
        }
    } else {
        // Fallback test
        double testRadius = 5.0;
        double volume = (4.0 / 3.0) * PI * std::pow(testRadius, 3);
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Radius: " << testRadius << ", Volume: " << volume << std::endl;
    }

    return 0;
}
