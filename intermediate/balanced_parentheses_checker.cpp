/*
 * Problem Description:
 * Write a program that checks whether a given string of parentheses is balanced.
 * It uses a stack to match opening and closing brackets ('(', ')', '{', '}', '[', ']').
 */

#include <iostream>
#include <stack>
#include <string>

bool isBalanced(std::string expr) {
    std::stack<char> s;
    char x;
    
    for (int i = 0; i < expr.length(); i++) {
        if (expr[i] == '(' || expr[i] == '[' || expr[i] == '{') {
            s.push(expr[i]);
            continue;
        }
        
        if (s.empty())
            return false;
            
        switch (expr[i]) {
            case ')':
                x = s.top();
                s.pop();
                if (x == '{' || x == '[')
                    return false;
                break;
            case '}':
                x = s.top();
                s.pop();
                if (x == '(' || x == '[')
                    return false;
                break;
            case ']':
                x = s.top();
                s.pop();
                if (x == '(' || x == '{')
                    return false;
                break;
        }
    }
    
    return (s.empty());
}

int main() {
    std::string expr;
    std::cout << "Enter an expression with parentheses: ";
    std::cin >> expr;

    if (isBalanced(expr))
        std::cout << "Balanced\n";
    else
        std::cout << "Not Balanced\n";
        
    return 0;
}
