#include <iostream>

int main() {
    int n1, n2;

    std::cout << "Enter two numbers: ";
    std::cin >> n1 >> n2;
    
    while(n1 != n2) {
        if(n1 > n2)
            n1 -= n2;
        else
            n2 -= n1;
    }

    std::cout << "HCF = " << n1 << "\n";
    
    return 0;
}
