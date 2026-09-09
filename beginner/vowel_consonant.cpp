#include <iostream>
#include <cctype>

int main() {
    char c;
    bool isLowercaseVowel, isUppercaseVowel;

    std::cout << "Enter an alphabet: ";
    std::cin >> c;

    isLowercaseVowel = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
    isUppercaseVowel = (c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');

    if (!isalpha(c)) {
        std::cout << "Error! Non-alphabetic character.\n";
    } else if (isLowercaseVowel || isUppercaseVowel) {
        std::cout << c << " is a vowel.\n";
    } else {
        std::cout << c << " is a consonant.\n";
    }

    return 0;
}
