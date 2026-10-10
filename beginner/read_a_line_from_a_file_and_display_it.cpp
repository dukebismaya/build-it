#include <iostream>
#include <fstream>
using namespace std;

int main() {
    char str[100];
    ifstream inFile;

    inFile.open("program.txt");

    if (!inFile) {
        cout << "Unable to open file program.txt";
        return 1; // exit with error
    }

    inFile.getline(str, 100);
    cout << "Read from file: " << str << endl;

    inFile.close();

    return 0;
}
