#include <iostream>
#include <stdlib.h>
using namespace std;

struct course {
    int marks;
    char subject[30];
};

int main() {
    struct course *ptr;
    int noOfRecords;
    cout << "Enter number of records: ";
    cin >> noOfRecords;

    ptr = (struct course *)malloc(noOfRecords * sizeof(struct course));
    for(int i = 0; i < noOfRecords; ++i) {
        cout << "Enter name of the subject and marks respectively:\n";
        cin >> (ptr + i)->subject >> (ptr + i)->marks;
    }

    cout << "Displaying Information:\n";
    for(int i = 0; i < noOfRecords; ++i)
        cout << (ptr + i)->subject << "\t" << (ptr + i)->marks << endl;

    free(ptr);

    return 0;
}
