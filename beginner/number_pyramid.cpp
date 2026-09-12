#include <iostream>

int main() {
    int rows;

    std::cout << "Enter number of rows: ";
    std::cin >> rows;

    for (int i = 1; i <= rows; ++i) {
        for (int space = 1; space <= rows - i; ++space) {
            std::cout << "  ";
        }
        for (int j = 1; j <= 2 * i - 1; ++j) {
            std::cout << j << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
