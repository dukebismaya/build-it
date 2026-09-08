#include <iostream>

int main() {
    int n1, n2, max;

    std::cout << "Enter two positive integers: ";
    std::cin >> n1 >> n2;

    max = (n1 > n2) ? n1 : n2;

    do {
        if (max % n1 == 0 && max % n2 == 0) {
            std::cout << "LCM of " << n1 << " and " << n2 << " is " << max << "\n";
            break;
        }
        else
            ++max;
    } while (true);

    return 0;
}
