#include <iostream>
#include <cmath>
using namespace std;

bool checkPrimeNumber(int n);
bool checkArmstrongNumber(int n);

int main() {
    int n;
    bool flag;

    cout << "Enter a positive integer: ";
    cin >> n;

    flag = checkPrimeNumber(n);
    if (flag)
        cout << n << " is a prime number." << endl;
    else
        cout << n << " is not a prime number." << endl;

    flag = checkArmstrongNumber(n);
    if (flag)
        cout << n << " is an Armstrong number." << endl;
    else
        cout << n << " is not an Armstrong number." << endl;

    return 0;
}

bool checkPrimeNumber(int n) {
    bool isPrime = true;
    if (n == 0 || n == 1) {
        isPrime = false;
    }
    else {
        for(int j = 2; j <= n / 2; ++j) {
            if (n % j == 0) {
                isPrime = false;
                break;
            }
        }
    }
    return isPrime;
}

bool checkArmstrongNumber(int num) {
    int originalNum, remainder, n = 0, result = 0, power;
    originalNum = num;

    while (originalNum != 0) {
        originalNum /= 10;
        ++n;
    }

    originalNum = num;

    while (originalNum != 0) {
        remainder = originalNum % 10;
        power = round(pow(remainder, n));
        result += power;
        originalNum /= 10;
    }

    if (result == num)
        return true;
    else
        return false;
}
