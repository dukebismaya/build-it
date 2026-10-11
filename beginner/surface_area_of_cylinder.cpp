#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    double radius, height;
    const double PI = 3.14159265358979323846;

    std::cout << "Enter radius and height of cylinder: ";
    if (std::cin >> radius >> height) {
        if (radius < 0 || height < 0) {
            std::cout << "Radius and height must be non-negative." << std::endl;
        } else {
            double curvedSurfaceArea = 2 * PI * radius * height;
            double totalSurfaceArea = 2 * PI * radius * (radius + height);
            double volume = PI * radius * radius * height;

            std::cout << std::fixed << std::setprecision(4);
            std::cout << "Curved Surface Area: " << curvedSurfaceArea << std::endl;
            std::cout << "Total Surface Area: " << totalSurfaceArea << std::endl;
            std::cout << "Volume: " << volume << std::endl;
        }
    } else {
        double r = 3.0, h = 7.0;
        double curvedSurfaceArea = 2 * PI * r * h;
        double totalSurfaceArea = 2 * PI * r * (r + h);
        double volume = PI * r * r * h;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Radius: " << r << ", Height: " << h << std::endl;
        std::cout << "Curved Surface Area: " << curvedSurfaceArea << std::endl;
        std::cout << "Total Surface Area: " << totalSurfaceArea << std::endl;
        std::cout << "Volume: " << volume << std::endl;
    }

    return 0;
}
