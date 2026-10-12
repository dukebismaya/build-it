#include <iostream>

// Function to check if a number is an Automorphic number
// An Automorphic number is a number whose square ends in the same digits as the number itself.
// Example: 5^2 = 25 (ends in 5), 25^2 = 625 (ends in 25), 76^2 = 5776 (ends in 76)
bool isAutomorphic(int n) {
    if (n < 0) return false;
    long long sq = 1LL * n * n;
    int temp = n;
    while (temp > 0) {
        if (temp % 10 != sq % 10) {
            return false;
        }
        temp /= 10;
        sq /= 10;
    }
    return true;
}

int main() {
    int num;
    std::cout << "Enter a number: ";
    if (std::cin >> num) {
        if (isAutomorphic(num)) {
            std::cout << num << " is an Automorphic number." << std::endl;
        } else {
            std::cout << num << " is not an Automorphic number." << std::endl;
        }
    } else {
        int testCases[] = {5, 6, 25, 76, 89, 376};
        for (int t : testCases) {
            std::cout << t << (isAutomorphic(t) ? " is an Automorphic number." : " is not an Automorphic number.") << std::endl;
        }
    }
    return 0;
}
