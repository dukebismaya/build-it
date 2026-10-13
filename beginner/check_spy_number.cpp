#include <iostream>

// Function to check if a number is a Spy number
// A Spy number is a number where the sum of all digits equals the product of all digits.
// Examples: 1124 -> sum=8, product=8  |  123 -> sum=6, product=6
bool isSpyNumber(int n) {
    if (n <= 0) return false;
    int sum = 0;
    int product = 1;
    while (n > 0) {
        int digit = n % 10;
        sum += digit;
        product *= digit;
        n /= 10;
    }
    return sum == product;
}

int main() {
    int num;
    std::cout << "Enter a positive integer: ";
    if (std::cin >> num) {
        if (isSpyNumber(num)) {
            std::cout << num << " is a Spy number." << std::endl;
        } else {
            std::cout << num << " is not a Spy number." << std::endl;
        }
    } else {
        int testCases[] = {1124, 123, 22, 1111, 145};
        for (int t : testCases) {
            std::cout << t << (isSpyNumber(t) ? " is a Spy number." : " is not a Spy number.") << std::endl;
        }
    }
    return 0;
}
