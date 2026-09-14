/*
 * Problem Description:
 * Write a program to reverse a string using a Stack data structure.
 */

#include <iostream>
#include <stack>
#include <string>

std::string reverseString(std::string str) {
    std::stack<char> s;
    for (char c : str) {
        s.push(c);
    }
    
    std::string reversed = "";
    while (!s.empty()) {
        reversed += s.top();
        s.pop();
    }
    
    return reversed;
}

int main() {
    std::string str;
    std::cout << "Enter a string: ";
    std::cin >> str;

    std::cout << "Reversed string: " << reverseString(str) << "\n";
    return 0;
}
