#include <iostream>

// Function to compute factorial of a digit (0 to 9)
long long factorial(int d) {
    long long fact = 1;
    for (int i = 2; i <= d; ++i) {
        fact *= i;
    }
    return fact;
}

// Function to check if a number is a Strong (Krishnamurthy) number
// A Strong number is a number whose sum of factorials of digits equals the number itself.
// Example: 145 = 1! + 4! + 5! = 1 + 24 + 120 = 145
bool isStrongNumber(int n) {
    if (n <= 0) return false;
    int temp = n;
    long long sumOfFactorials = 0;
    while (temp > 0) {
        int digit = temp % 10;
        sumOfFactorials += factorial(digit);
        temp /= 10;
    }
    return sumOfFactorials == n;
}

int main() {
    int num;
    std::cout << "Enter a positive integer: ";
    if (std::cin >> num) {
        if (isStrongNumber(num)) {
            std::cout << num << " is a Strong number." << std::endl;
        } else {
            std::cout << num << " is not a Strong number." << std::endl;
        }
    } else {
        int testCases[] = {1, 2, 145, 40585, 120};
        for (int t : testCases) {
            std::cout << t << (isStrongNumber(t) ? " is a Strong number." : " is not a Strong number.") << std::endl;
        }
    }
    return 0;
}
