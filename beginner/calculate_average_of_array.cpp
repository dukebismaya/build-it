#include <iostream>

int main() {
    int n, i;
    float num[100], sum = 0.0, average;

    std::cout << "Enter the numbers of data: ";
    std::cin >> n;

    while (n > 100 || n <= 0) {
        std::cout << "Error! number should in range of (1 to 100).\n";
        std::cout << "Enter the number again: ";
        std::cin >> n;
    }

    for(i = 0; i < n; ++i) {
        std::cout << i + 1 << ". Enter number: ";
        std::cin >> num[i];
        sum += num[i];
    }

    average = sum / n;
    std::cout << "Average = " << average << "\n";

    return 0;
}
