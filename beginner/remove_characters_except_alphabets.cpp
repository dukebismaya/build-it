#include <iostream>
#include <string>

int main() {
    std::string line, temp = "";

    std::cout << "Enter a string: ";
    std::getline(std::cin, line);

    for (int i = 0; i < line.size(); ++i) {
        if ((line[i] >= 'a' && line[i] <= 'z') || (line[i] >= 'A' && line[i] <= 'Z')) {
            temp = temp + line[i];
        }
    }
    line = temp;
    
    std::cout << "Output String: " << line << "\n";
    return 0;
}
