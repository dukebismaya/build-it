#include <iostream>
#include <string>

int main() {
    std::string str, reversedStr = "";
    
    std::cout << "Enter a string: ";
    std::cin >> str;
    
    for (int i = str.length() - 1; i >= 0; --i) {
        reversedStr += str[i];
    }
    
    std::cout << "Reversed string: " << reversedStr << "\n";
    
    return 0;
}
