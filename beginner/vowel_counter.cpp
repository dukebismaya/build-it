#include <iostream>
#include <string>
#include <cctype>

int countVowels(const std::string& str) {
    int count = 0;
    for (char c : str) {
        char lower = std::tolower(c);
        if (lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' || lower == 'u') {
            count++;
        }
    }
    return count;
}

int main() {
    std::string input;
    std::cout << "Enter a string: ";
    std::getline(std::cin, input);
    
    std::cout << "Number of vowels: " << countVowels(input) << "\n";
    return 0;
}
