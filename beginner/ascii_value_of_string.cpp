#include <iostream>
#include <string>

int main() {
    std::string str;
    std::cout << "Enter a string: ";
    std::cin >> str;

    std::cout << "ASCII values:\n";
    for (char c : str) {
        std::cout << c << ": " << int(c) << "\n";
    }

    return 0;
}
