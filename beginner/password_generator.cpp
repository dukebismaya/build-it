#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

std::string generatePassword(int length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*";
    std::string password = "";
    
    for (int i = 0; i < length; ++i) {
        password += characters[std::rand() % characters.length()];
    }
    
    return password;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int length;
    
    std::cout << "Enter the desired password length: ";
    std::cin >> length;
    
    if (length <= 0) {
        std::cout << "Length must be greater than 0.\n";
    } else {
        std::cout << "Generated Password: " << generatePassword(length) << "\n";
    }
    
    return 0;
}
