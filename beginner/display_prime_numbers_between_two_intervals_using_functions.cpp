#include <iostream>
using namespace std;

bool checkPrimeNumber(int);

int main() {
    int n1, n2;
    bool flag;

    cout << "Enter two positive integers: ";
    cin >> n1 >> n2;

    cout << "Prime numbers between " << n1 << " and " << n2 << " are: ";

    for(int i = n1 + 1; i < n2; ++i) {
        flag = checkPrimeNumber(i);

        if(flag)
            cout << i << " ";
    }
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
