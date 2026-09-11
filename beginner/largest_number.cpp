#include <iostream>

int main() {
    double n1, n2, n3;

    std::cout << "Enter three numbers: ";
    std::cin >> n1 >> n2 >> n3;

    if(n1 >= n2 && n1 >= n3)
        std::cout << "Largest number: " << n1 << "\n";
    else if(n2 >= n1 && n2 >= n3)
        std::cout << "Largest number: " << n2 << "\n";
    else
        std::cout << "Largest number: " << n3 << "\n";
  
    return 0;
}
