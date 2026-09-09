#include <iostream>

int main() {
    int n, reversedN = 0, remainder, originalN;

    std::cout << "Enter an integer: ";
    std::cin >> n;

    originalN = n;

    while (n != 0) {
        remainder = n % 10;
        reversedN = reversedN * 10 + remainder;
        n /= 10;
    }

    if (originalN == reversedN)
        std::cout << originalN << " is a palindrome.\n";
    else
        std::cout << originalN << " is not a palindrome.\n";

    return 0;
}
