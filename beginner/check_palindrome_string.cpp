#include <iostream>
#include <string>
using namespace std;

int main() {
    string str, rev_str = "";
    
    cout << "Enter a string: ";
    cin >> str;

    for (int i = str.length() - 1; i >= 0; i--) {
        rev_str += str[i];
    }

    if (str == rev_str) {
        cout << str << " is a palindrome." << endl;
    } else {
        cout << str << " is not a palindrome." << endl;
    }

    return 0;
}
