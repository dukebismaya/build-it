#include <iostream>

int main() {
    long long n;
    int count = 0;

    std::cout << "Enter an integer: ";
    std::cin >> n;

    if (n == 0) {
        count = 1;
    } else {
        while (n != 0) {
            n /= 10;
            ++count;
        }
    }

    std::cout << "Number of digits: " << count << "\n";

    return 0;
}
