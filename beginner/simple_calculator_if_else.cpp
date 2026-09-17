#include <iostream>

int main() {
    char op;
    float num1, num2;

    std::cout << "Enter operator (+, -, *, /): ";
    std::cin >> op;

    std::cout << "Enter two operands: ";
    std::cin >> num1 >> num2;

    if (op == '+')
        std::cout << num1 << " + " << num2 << " = " << num1 + num2 << "\n";
    else if (op == '-')
        std::cout << num1 << " - " << num2 << " = " << num1 - num2 << "\n";
    else if (op == '*')
        std::cout << num1 << " * " << num2 << " = " << num1 * num2 << "\n";
    else if (op == '/')
        std::cout << num1 << " / " << num2 << " = " << num1 / num2 << "\n";
    else
        std::cout << "Error! operator is not correct\n";

    return 0;
}
