#include <iostream>

int main() {
    char ch;
    bool isLowercaseVowel, isUppercaseVowel;

    std::cout << "Enter an alphabet: ";
    std::cin >> ch;

    isLowercaseVowel = (ch == 'a' || ch == 'e' || ch == 'i' || ch == 'o' || ch == 'u');
    isUppercaseVowel = (ch == 'A' || ch == 'E' || ch == 'I' || ch == 'O' || ch == 'U');

    if (isLowercaseVowel || isUppercaseVowel)
        std::cout << ch << " is a vowel.\n";
    else
        std::cout << ch << " is a consonant.\n";

    return 0;
}
