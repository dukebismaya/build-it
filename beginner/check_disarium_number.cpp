#include <iostream>
#include <cmath>

// Function to check if a number is a Disarium number
// A Disarium number is a number where sum of digits powered with their positions equals the number.
// Example: 175 -> 1^1 + 7^2 + 5^3 = 1 + 49 + 125 = 175
bool isDisarium(int n) {
    if (n < 0) return false;

    // Count number of digits
    int temp = n;
    int digits = 0;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }
    if (n == 0) return true;

    // Calculate sum of digits raised to their positions
    temp = n;
    long long sum = 0;
    int pos = digits;
    while (temp > 0) {
        int digit = temp % 10;
        sum += static_cast<long long>(std::pow(digit, pos));
        temp /= 10;
        pos--;
    }
    return sum == n;
}

int main() {
    int num;
    std::cout << "Enter a number: ";
    if (std::cin >> num) {
        if (isDisarium(num)) {
            std::cout << num << " is a Disarium number." << std::endl;
        } else {
            std::cout << num << " is not a Disarium number." << std::endl;
        }
    } else {
        int testCases[] = {89, 135, 175, 518, 120};
        for (int t : testCases) {
            std::cout << t << (isDisarium(t) ? " is a Disarium number." : " is not a Disarium number.") << std::endl;
        }
    }
    return 0;
}
