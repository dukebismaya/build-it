#include <iostream>
#include <string>

int main() {
    std::string str;
    int length = 0;

    std::cout << "Enter a string: ";
    std::getline(std::cin, str);

    for (char c : str) {
        length++;
    }

    std::cout << "Length of the string is: " << length << "\n";

    return 0;
}
