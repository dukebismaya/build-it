#include <iostream>

void swap(int* n1, int* n2) {
    int temp;
    temp = *n1;
    *n1 = *n2;
    *n2 = temp;
}

int main() {
    int a = 1, b = 2;

    std::cout << "Before swapping\n";
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";

    swap(&a, &b);

    std::cout << "\nAfter swapping\n";
    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";

    return 0;
}
