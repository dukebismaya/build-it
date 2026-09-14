#include <iostream>

int main() {
    double length, width, area;

    std::cout << "Enter the length of the rectangle: ";
    std::cin >> length;
    std::cout << "Enter the width of the rectangle: ";
    std::cin >> width;

    area = length * width;

    std::cout << "Area of the rectangle = " << area << "\n";

    return 0;
}
