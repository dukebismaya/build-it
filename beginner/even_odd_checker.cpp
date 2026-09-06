#include <iostream>

int main() {
    int number;
    
    std::cout << "Enter an integer: ";
    std::cin >> number;
    
    if (number % 2 == 0) {
        std::cout << number << " is even.\n";
    } else {
        std::cout << number << " is odd.\n";
    }
    
    return 0;
}
