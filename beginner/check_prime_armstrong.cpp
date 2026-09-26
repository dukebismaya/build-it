#include <iostream>
#include <cmath>
using namespace std;

int checkPrimeNumber(int);
int checkArmstrongNumber(int);

int main() {
    int n, flag;

    cout << "Enter a positive integer: ";
    cin >> n;

    // check prime number
    flag = checkPrimeNumber(n);
    if (flag == 1)
        cout << n << " is a prime number.\n";
    else
        cout << n << " is not a prime number.\n";

    // check Armstrong number
    flag = checkArmstrongNumber(n);
    if (flag == 1)
        cout << n << " is an Armstrong number.";
    else
        cout << n << " is not an Armstrong number.";
    return 0;
}

int checkPrimeNumber(int n) {
    int i, flag = 1, isPrime = 1;

    // 0 and 1 are not prime numbers
    if (n == 0 || n == 1) {
        isPrime = 0;
    }
    else {
        for(i = 2; i <= n/2; ++i) {
            if(n % i == 0) {
                isPrime = 0;
                break;
            }
        }
    }
    return isPrime;
}

int checkArmstrongNumber(int num) {
    int originalNum, remainder, n = 0, flag = 0;
    double result = 0.0;

    originalNum = num;

    for (originalNum = num; originalNum != 0; ++n) {
        originalNum /= 10;
    }

    for (originalNum = num; originalNum != 0; originalNum /= 10) {
        remainder = originalNum % 10;
        result += pow(remainder, n);
    }

    if (round(result) == num)
        flag = 1;

    return flag;
}
