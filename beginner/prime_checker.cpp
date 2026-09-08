#include <iostream>

int main() {
    int n, i;
    bool isPrime = true;

    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    if (n == 0 || n == 1) {
        isPrime = false;
    } else {
        for (i = 2; i <= n / 2; ++i) {
            if (n % i == 0) {
                isPrime = false;
                break;
            }
        }
    }

    if (isPrime)
        std::cout << n << " is a prime number.\n";
    else
        std::cout << n << " is not a prime number.\n";

    return 0;
}
