#include <iostream>
#include <cstdlib>
#include <ctime>

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int secretNumber = std::rand() % 100 + 1;
    int guess;
    int attempts = 0;
    
    std::cout << "Guess the number (between 1 and 100):\n";
    
    do {
        std::cout << "Enter your guess: ";
        std::cin >> guess;
        attempts++;
        
        if (guess > secretNumber) {
            std::cout << "Too high!\n";
        } else if (guess < secretNumber) {
            std::cout << "Too low!\n";
        } else {
            std::cout << "Congratulations! You guessed it in " << attempts << " attempts.\n";
        }
    } while (guess != secretNumber);
    
    return 0;
}
