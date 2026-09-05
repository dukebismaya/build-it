#include <iostream>
#include <chrono>
#include <string>

int main() {
    std::string input;
    
    std::cout << "Press Enter to start the stopwatch...";
    std::getline(std::cin, input);
    auto start = std::chrono::high_resolution_clock::now();
    
    std::cout << "Stopwatch started. Press Enter to stop...";
    std::getline(std::cin, input);
    auto stop = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Elapsed time: " << duration.count() / 1000.0 << " seconds\n";
    
    return 0;
}
