#include <iostream>

// Function to check if a number is a Neon number
// A Neon number is a number where the sum of digits of square of the number is equal to the number.
// Example: 9 -> 9^2 = 81 -> 8 + 1 = 9
bool isNeonNumber(int n) {
    if (n < 0) return false;
    long long sq = 1LL * n * n;
    long long sumOfDigits = 0;
    while (sq > 0) {
        sumOfDigits += sq % 10;
        sq /= 10;
    }
    return sumOfDigits == n;
}

int main() {
    int num;
    std::cout << "Enter a number: ";
    if (std::cin >> num) {
        if (isNeonNumber(num)) {
            std::cout << num << " is a Neon number." << std::endl;
        } else {
            std::cout << num << " is not a Neon number." << std::endl;
        }
    } else {
        int testCases[] = {0, 1, 9, 12, 45};
        for (int t : testCases) {
            std::cout << t << (isNeonNumber(t) ? " is a Neon number." : " is not a Neon number.") << std::endl;
        }
    }
    return 0;
}
