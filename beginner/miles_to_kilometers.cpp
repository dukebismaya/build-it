#include <iostream>

int main() {
    double miles, kilometers;
    std::cout << "Enter distance in miles: ";
    std::cin >> miles;
    
    kilometers = miles * 1.60934;
    
    std::cout << miles << " miles is equal to " << kilometers << " kilometers.\n";
    return 0;
}
