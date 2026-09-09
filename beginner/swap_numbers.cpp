#include <iostream>

int main() {
    int a = 5, b = 10, temp;

    std::cout << "Before swapping.\n";
    std::cout << "a = " << a << ", b = " << b << "\n";

    temp = a;
    a = b;
    b = temp;

    std::cout << "\nAfter swapping.\n";
    std::cout << "a = " << a << ", b = " << b << "\n";

    return 0;
}
