#include <iostream>

int main() {
    int n;
    double num, sum = 0.0, average;

    std::cout << "Enter the number of elements: ";
    std::cin >> n;

    for(int i = 1; i <= n; ++i) {
        std::cout << i << ". Enter number: ";
        std::cin >> num;
        sum += num;
    }

    average = sum / n;
    std::cout << "Average = " << average << "\n";

    return 0;
}
