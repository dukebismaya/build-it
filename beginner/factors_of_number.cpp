#include <iostream>

int main() {
    int n;
    std::cout << "Enter a positive integer: ";
    std::cin >> n;

    std::cout << "Factors of " << n << " are: ";  
    for(int i = 1; i <= n; ++i) {
        if(n % i == 0)
            std::cout << i << " ";
    }
    std::cout << "\n";

    return 0;
}
