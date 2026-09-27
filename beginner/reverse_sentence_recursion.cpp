#include <iostream>
using namespace std;

void reverse();

int main() {
    cout << "Enter a sentence: ";
    reverse();
    return 0;
}

void reverse() {
    char c;
    cin.get(c);
    if(c != '\n') {
        reverse();
        cout << c;
    }
}
