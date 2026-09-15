#include <iostream>

int main() {
    int n, sum = 0;

    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    int i = 1;
    while (i <= n) {
        sum += i;
        ++i;
    }

    std::cout << "Sum = " << sum << "\n";

    return 0;
}
