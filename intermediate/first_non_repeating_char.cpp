/*
 * Problem Description:
 * Find the first non-repeating character in a given string. The program uses a hash map
 * to count occurrences of each character and then iterates over the string to find the first one with count 1.
 */

#include <iostream>
#include <string>
#include <unordered_map>

char firstNonRepeatingChar(std::string str) {
    std::unordered_map<char, int> countMap;
    
    for (char c : str) {
        countMap[c]++;
    }
    
    for (char c : str) {
        if (countMap[c] == 1) {
            return c;
        }
    }
    
    return '\0'; // Return null char if none exists
}

int main() {
    std::string str;
    std::cout << "Enter a string: ";
    std::cin >> str;

    char result = firstNonRepeatingChar(str);
    if (result == '\0') {
        std::cout << "No non-repeating character found.\n";
    } else {
        std::cout << "First non-repeating character is: " << result << "\n";
    }
    
    return 0;
}
