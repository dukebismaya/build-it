#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int numDice;
    
    std::cout << "How many 6-sided dice would you like to roll? ";
    std::cin >> numDice;
    
    if (numDice <= 0) {
        std::cout << "You must roll at least one die.\n";
        return 1;
    }
    
    int total = 0;
    std::cout << "Rolling " << numDice << " dice...\n";
    
    for (int i = 1; i <= numDice; ++i) {
        int roll = std::rand() % 6 + 1;
        std::cout << "Die " << i << ": " << roll << "\n";
        total += roll;
    }
    
    std::cout << "Total sum: " << total << "\n";
    
    return 0;
}
