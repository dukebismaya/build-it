#include <iostream>

int main() {
    char c;
    
    std::cout << "Enter a character: ";
    std::cin >> c;
    
    std::cout << "ASCII Value of " << c << " is " << int(c) << "\n";
    
    return 0;
}
