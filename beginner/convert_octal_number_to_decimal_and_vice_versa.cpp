#include <iostream>
#include <cmath>
using namespace std;

int octalToDecimal(int octalNumber);
int decimalToOctal(int decimalNumber);

int main() {
    int octalNumber, decimalNumber;

    cout << "Enter an octal number: ";
    cin >> octalNumber;
    cout << octalNumber << " in octal = " << octalToDecimal(octalNumber) << " in decimal" << endl;

    cout << "Enter a decimal number: ";
    cin >> decimalNumber;
    cout << decimalNumber << " in decimal = " << decimalToOctal(decimalNumber) << " in octal";

    return 0;
}

int octalToDecimal(int octalNumber) {
    int decimalNumber = 0, i = 0, rem;
    while (octalNumber != 0) {
        rem = octalNumber % 10;
        octalNumber /= 10;
        decimalNumber += rem * pow(8, i);
        ++i;
    }
    return decimalNumber;
}

int decimalToOctal(int decimalNumber) {
    int octalNumber = 0, i = 1;

    while (decimalNumber != 0) {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8;
        i *= 10;
    }

    return octalNumber;
}
