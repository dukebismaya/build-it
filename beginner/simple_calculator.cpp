#include <iostream>

int main() {
    char op;
    double num1, num2;

    std::cout << "Enter operator (+, -, *, /): ";
    std::cin >> op;

    std::cout << "Enter two operands: ";
    std::cin >> num1 >> num2;

    switch(op) {
        case '+':
            std::cout << num1 << " + " << num2 << " = " << num1 + num2 << "\n";
            break;
        case '-':
            std::cout << num1 << " - " << num2 << " = " << num1 - num2 << "\n";
            break;
        case '*':
            std::cout << num1 << " * " << num2 << " = " << num1 * num2 << "\n";
            break;
        case '/':
            if (num2 != 0.0)
                std::cout << num1 << " / " << num2 << " = " << num1 / num2 << "\n";
            else
                std::cout << "Division by zero is not allowed.\n";
            break;
        default:
            std::cout << "Error! Operator is not correct\n";
            break;
    }

    return 0;
}
