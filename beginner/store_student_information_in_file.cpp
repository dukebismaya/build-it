#include <iostream>
#include <fstream>
using namespace std;

int main() {
    char name[50];
    int marks, i, num;

    cout << "Enter number of students: ";
    cin >> num;

    ofstream outfile("student.txt");

    if (!outfile.is_open()) {
        cout << "Error opening file!";
        return 1;
    }

    for (i = 0; i < num; ++i) {
        cout << "For student " << i + 1 << "\nEnter name: ";
        cin >> name;
        cout << "Enter marks: ";
        cin >> marks;
        outfile << name << endl << marks << endl;
    }

    outfile.close();
    cout << "Data stored successfully in student.txt\n";
    return 0;
}
