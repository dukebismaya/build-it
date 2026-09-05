#include <iostream>

int main() {
    double weight, height;
    
    std::cout << "Enter weight in kilograms: ";
    std::cin >> weight;
    
    std::cout << "Enter height in meters: ";
    std::cin >> height;
    
    if (height <= 0) {
        std::cout << "Height must be greater than 0.\n";
        return 1;
    }
    
    double bmi = weight / (height * height);
    std::cout << "Your BMI is: " << bmi << "\n";
    
    if (bmi < 18.5) {
        std::cout << "Category: Underweight\n";
    } else if (bmi < 24.9) {
        std::cout << "Category: Normal weight\n";
    } else if (bmi < 29.9) {
        std::cout << "Category: Overweight\n";
    } else {
        std::cout << "Category: Obese\n";
    }
    
    return 0;
}
