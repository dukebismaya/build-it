#include <iostream>

int main() {
    char c;
    std::cout << "Enter a character: ";
    std::cin >> c;

    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        std::cout << c << " is an alphabet.\n";
    else
        std::cout << c << " is not an alphabet.\n";

    return 0;
}
