#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    string line;
    ifstream infile("student.txt");

    if (!infile.is_open()) {
        cout << "Error opening file (make sure student.txt exists)";
        return 1;
    }
    
    cout << "Reading from file:\n";
    while (getline(infile, line)) {
        cout << line << endl;
    }

    infile.close();
    return 0;
}
