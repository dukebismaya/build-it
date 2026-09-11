#include <iostream>
#include <cmath>

int main() {
    float a, b, c, x1, x2, discriminant, realPart, imaginaryPart;
    std::cout << "Enter coefficients a, b and c: ";
    std::cin >> a >> b >> c;
    
    discriminant = b*b - 4*a*c;
    
    if (discriminant > 0) {
        x1 = (-b + std::sqrt(discriminant)) / (2*a);
        x2 = (-b - std::sqrt(discriminant)) / (2*a);
        std::cout << "Roots are real and different.\n";
        std::cout << "x1 = " << x1 << "\n";
        std::cout << "x2 = " << x2 << "\n";
    }
    else if (discriminant == 0) {
        std::cout << "Roots are real and same.\n";
        x1 = -b/(2*a);
        std::cout << "x1 = x2 =" << x1 << "\n";
    }
    else {
        realPart = -b/(2*a);
        imaginaryPart = std::sqrt(-discriminant)/(2*a);
        std::cout << "Roots are complex and different.\n";
        std::cout << "x1 = " << realPart << "+" << imaginaryPart << "i\n";
        std::cout << "x2 = " << realPart << "-" << imaginaryPart << "i\n";
    }
    return 0;
}
