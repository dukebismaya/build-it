#include <iostream>

// Function to find the digital root of a number
// The digital root is the single-digit value obtained by repeatedly summing digits.
// Example: 9875 -> 9+8+7+5 = 29 -> 2+9 = 11 -> 1+1 = 2
// Direct formula: digitalRoot(n) = 1 + (n - 1) % 9 for n > 0
int digitalRoot(int n) {
    if (n == 0) return 0;
    n = std::abs(n);
    return 1 + (n - 1) % 9;
}

// Iterative method for verification
int digitalRootIterative(int n) {
    n = std::abs(n);
    while (n >= 10) {
        int sum = 0;
        while (n > 0) {
            sum += n % 10;
            n /= 10;
        }
        n = sum;
    }
    return n;
}

int main() {
    int num;
    std::cout << "Enter a number: ";
    if (std::cin >> num) {
        std::cout << "Digital root of " << num << " (formula): " << digitalRoot(num) << std::endl;
        std::cout << "Digital root of " << num << " (iterative): " << digitalRootIterative(num) << std::endl;
    } else {
        int testCases[] = {0, 5, 9875, 12345, 99999};
        for (int t : testCases) {
            std::cout << "Digital root of " << t << ": " << digitalRoot(t)
                      << " (verify: " << digitalRootIterative(t) << ")" << std::endl;
        }
    }
    return 0;
}
