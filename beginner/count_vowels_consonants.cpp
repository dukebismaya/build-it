#include <iostream>
#include <string>

int main() {
    std::string str;
    int vowels = 0, consonants = 0;

    std::cout << "Enter a string: ";
    std::getline(std::cin, str);

    for (int i = 0; i < str.length(); ++i) {
        if (str[i] == 'a' || str[i] == 'e' || str[i] == 'i' ||
            str[i] == 'o' || str[i] == 'u' || str[i] == 'A' ||
            str[i] == 'E' || str[i] == 'I' || str[i] == 'O' ||
            str[i] == 'U') {
            ++vowels;
        } else if ((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z')) {
            ++consonants;
        }
    }

    std::cout << "Vowels: " << vowels << "\n";
    std::cout << "Consonants: " << consonants << "\n";

    return 0;
}
