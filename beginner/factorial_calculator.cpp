#include <iostream>

int main() {
    int n;
    long long factorial = 1;

    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    if (n < 0) {
        std::cout << "Error! Factorial of a negative number doesn't exist.\n";
    } else {
        for(int i = 1; i <= n; ++i) {
            factorial *= i;
        }
        std::cout << "Factorial of " << n << " = " << factorial << "\n";
    }

    return 0;
}
