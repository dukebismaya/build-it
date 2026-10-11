#include <iostream>

// Function to calculate sum of digits
int sumOfDigits(int n) {
    int sum = 0;
    n = std::abs(n);
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

// Function to check if a number is a Harshad (Niven) number
bool isHarshad(int n) {
    if (n == 0) return false;
    int sum = sumOfDigits(n);
    return (n % sum == 0);
}

int main() {
    int num;
    std::cout << "Enter a positive integer: ";
    if (std::cin >> num) {
        if (num <= 0) {
            std::cout << "Please enter a positive integer." << std::endl;
        } else if (isHarshad(num)) {
            std::cout << num << " is a Harshad number (divisible by " << sumOfDigits(num) << ")." << std::endl;
        } else {
            std::cout << num << " is not a Harshad number." << std::endl;
        }
    } else {
        int testCases[] = {18, 21, 25, 1729};
        for (int t : testCases) {
            std::cout << t << (isHarshad(t) ? " is a Harshad number." : " is not a Harshad number.") << std::endl;
        }
    }

    return 0;
}
