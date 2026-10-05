#include <iostream>
#include <cmath>
using namespace std;

int convertBinarytoOctal(long long);
long long convertOctalToBinary(int);

int main() {
    long long binaryNumber;
    int octalNumber;

    cout << "Enter a binary number: ";
    cin >> binaryNumber;
    cout << binaryNumber << " in binary = " << convertBinarytoOctal(binaryNumber) << " in octal" << endl;

    cout << "Enter an octal number: ";
    cin >> octalNumber;
    cout << octalNumber << " in octal = " << convertOctalToBinary(octalNumber) << " in binary";

    return 0;
}

int convertBinarytoOctal(long long binaryNumber) {
    int octalNumber = 0, decimalNumber = 0, i = 0;

    while(binaryNumber != 0) {
        decimalNumber += (binaryNumber % 10) * pow(2, i);
        ++i;
        binaryNumber /= 10;
    }

    i = 1;

    while (decimalNumber != 0) {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8;
        i *= 10;
    }

    return octalNumber;
}

long long convertOctalToBinary(int octalNumber) {
    int decimalNumber = 0, i = 0;
    long long binaryNumber = 0;

    while(octalNumber != 0) {
        decimalNumber += (octalNumber % 10) * pow(8, i);
        ++i;
        octalNumber /= 10;
    }

    i = 1;

    while (decimalNumber != 0) {
        binaryNumber += (decimalNumber % 2) * i;
        decimalNumber /= 2;
        i *= 10;
    }

    return binaryNumber;
}
