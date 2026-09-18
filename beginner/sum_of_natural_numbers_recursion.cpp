#include <iostream>

int add(int n);

int main() {
    int n;
    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    std::cout << "Sum =  " << add(n) << "\n";

    return 0;
}

int add(int n) {
    if(n != 0)
        return n + add(n - 1);
    return 0;
}
