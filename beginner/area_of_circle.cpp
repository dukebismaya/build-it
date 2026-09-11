#include <iostream>

int main() {
    double radius, area;
    const double PI = 3.14159;

    std::cout << "Enter the radius of the circle: ";
    std::cin >> radius;

    area = PI * radius * radius;

    std::cout << "Area of the circle = " << area << "\n";

    return 0;
}
