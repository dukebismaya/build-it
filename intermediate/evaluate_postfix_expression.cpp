/*
 * Problem Description:
 * Evaluate a given Postfix expression using a Stack. The program iterates over each
 * character, pushing numbers to the stack and performing operations on popped numbers.
 */

#include <iostream>
#include <stack>
#include <string>
#include <cctype>

int evaluatePostfix(std::string exp) {
    std::stack<int> st;

    for (int i = 0; i < exp.length(); ++i) {
        if (isdigit(exp[i])) {
            st.push(exp[i] - '0');
        } else {
            int val1 = st.top(); st.pop();
            int val2 = st.top(); st.pop();

            switch (exp[i]) {
                case '+': st.push(val2 + val1); break;
                case '-': st.push(val2 - val1); break;
                case '*': st.push(val2 * val1); break;
                case '/': st.push(val2 / val1); break;
            }
        }
    }
    return st.top();
}

int main() {
    std::string exp;
    std::cout << "Enter postfix expression (e.g., 231*+9-): ";
    std::cin >> exp;

    std::cout << "Postfix evaluation result: " << evaluatePostfix(exp) << "\n";
    
    return 0;
}
