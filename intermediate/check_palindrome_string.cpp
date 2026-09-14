/*
 * Problem Description:
 * Check if a given string is a palindrome. A palindrome string is read the same forwards
 * and backwards. This solution compares characters from the ends towards the center.
 */

#include <iostream>
#include <string>

bool isPalindrome(std::string str) {
    int left = 0;
    int right = str.length() - 1;
    
    while (left < right) {
        if (str[left] != str[right])
            return false;
        left++;
        right--;
    }
    return true;
}

int main() {
    std::string str;
    std::cout << "Enter a string: ";
    std::cin >> str;

    if (isPalindrome(str))
        std::cout << str << " is a palindrome.\n";
    else
        std::cout << str << " is not a palindrome.\n";

    return 0;
}
