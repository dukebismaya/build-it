/*
 * Problem Description:
 * Check whether two strings are anagrams of each other. An anagram of a string is another 
 * string that contains the same characters, only the order of characters can be different.
 */

#include <iostream>
#include <string>
#include <algorithm>

bool areAnagrams(std::string str1, std::string str2) {
    if (str1.length() != str2.length())
        return false;
        
    std::sort(str1.begin(), str1.end());
    std::sort(str2.begin(), str2.end());
    
    for (int i = 0; i < str1.length(); i++)
        if (str1[i] != str2[i])
            return false;
            
    return true;
}

int main() {
    std::string str1, str2;
    std::cout << "Enter first string: ";
    std::cin >> str1;
    std::cout << "Enter second string: ";
    std::cin >> str2;

    if (areAnagrams(str1, str2))
        std::cout << "The two strings are anagrams of each other.\n";
    else
        std::cout << "The two strings are not anagrams of each other.\n";

    return 0;
}
