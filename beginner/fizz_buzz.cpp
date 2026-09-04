#include <iostream>

void fizzBuzz(int n) {
    for (int i = 1; i <= n; ++i) {
        if (i % 15 == 0) {
            std::cout << "FizzBuzz\n";
        } else if (i % 3 == 0) {
            std::cout << "Fizz\n";
        } else if (i % 5 == 0) {
            std::cout << "Buzz\n";
        } else {
            std::cout << i << "\n";
        }
    }
}

int main() {
    std::cout << "FizzBuzz up to 20:\n";
    fizzBuzz(20);
    return 0;
}
