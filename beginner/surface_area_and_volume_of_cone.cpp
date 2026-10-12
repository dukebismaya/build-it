#include <iostream>
#include <iomanip>
#include <cmath>

int main() {
    double radius, height;
    const double PI = 3.14159265358979323846;

    std::cout << "Enter radius and height of the cone: ";
    if (std::cin >> radius >> height) {
        if (radius < 0 || height < 0) {
            std::cout << "Radius and height must be non-negative." << std::endl;
        } else {
            double slantHeight = std::sqrt(radius * radius + height * height);
            double curvedSurfaceArea = PI * radius * slantHeight;
            double totalSurfaceArea = PI * radius * (radius + slantHeight);
            double volume = (1.0 / 3.0) * PI * radius * radius * height;

            std::cout << std::fixed << std::setprecision(4);
            std::cout << "Slant Height: " << slantHeight << std::endl;
            std::cout << "Curved Surface Area: " << curvedSurfaceArea << std::endl;
            std::cout << "Total Surface Area: " << totalSurfaceArea << std::endl;
            std::cout << "Volume: " << volume << std::endl;
        }
    } else {
        double r = 4.0, h = 3.0;
        double slantHeight = std::sqrt(r * r + h * h);
        double curvedSurfaceArea = PI * r * slantHeight;
        double totalSurfaceArea = PI * r * (r + slantHeight);
        double volume = (1.0 / 3.0) * PI * r * r * h;

        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Radius: " << r << ", Height: " << h << std::endl;
        std::cout << "Slant Height: " << slantHeight << std::endl;
        std::cout << "Curved Surface Area: " << curvedSurfaceArea << std::endl;
        std::cout << "Total Surface Area: " << totalSurfaceArea << std::endl;
        std::cout << "Volume: " << volume << std::endl;
    }

    return 0;
}
