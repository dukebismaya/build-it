#include <iostream>
using namespace std;

typedef struct complex {
    float real;
    float imag;
} complexNumber;

complexNumber addComplexNumbers(complex, complex);

int main() {
    complexNumber n1, n2, tempData;

    cout << "For 1st complex number," << endl;
    cout << "Enter real and imaginary parts respectively:" << endl;
    cin >> n1.real >> n1.imag;

    cout << endl << "For 2nd complex number," << endl;
    cout << "Enter real and imaginary parts respectively:" << endl;
    cin >> n2.real >> n2.imag;

    tempData = addComplexNumbers(n1, n2);

    cout << endl << "Sum = " << tempData.real << " + " << tempData.imag << "i";

    return 0;
}

complexNumber addComplexNumbers(complex n1, complex n2) {
    complex temp;
    temp.real = n1.real + n2.real;
    temp.imag = n1.imag + n2.imag;
    return (temp);
}
