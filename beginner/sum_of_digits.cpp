#include <iostream>

int main() {
    int number, sum = 0, remainder;

    std::cout << "Enter an integer: ";
    std::cin >> number;
    
    int original = number;
    
    if (number < 0) {
        number = -number;
    }

    while (number > 0) {
        remainder = number % 10;
        sum += remainder;
        number /= 10;
    }

    std::cout << "Sum of the digits of " << original << " is: " << sum << "\n";

    return 0;
}
