#include <iostream>

int main() {
    double radius, perimeter;
    const double PI = 3.14159;

    std::cout << "Enter the radius of the circle: ";
    std::cin >> radius;

    perimeter = 2 * PI * radius;

    std::cout << "Perimeter of the circle = " << perimeter << "\n";

    return 0;
}
