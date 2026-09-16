#include <iostream>

int main() {
    double base, height, area;

    std::cout << "Enter the base of the triangle: ";
    std::cin >> base;
    std::cout << "Enter the height of the triangle: ";
    std::cin >> height;

    area = (base * height) / 2;

    std::cout << "Area of the triangle = " << area << "\n";

    return 0;
}
