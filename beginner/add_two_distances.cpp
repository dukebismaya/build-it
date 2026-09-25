#include <iostream>
using namespace std;

struct Distance {
    int feet;
    float inch;
} d1, d2, sumOfDistances;

int main() {
    cout << "Enter 1st distance," << endl;
    cout << "Enter feet: ";
    cin >> d1.feet;
    cout << "Enter inch: ";
    cin >> d1.inch;

    cout << "\nEnter 2nd distance," << endl;
    cout << "Enter feet: ";
    cin >> d2.feet;
    cout << "Enter inch: ";
    cin >> d2.inch;

    sumOfDistances.feet = d1.feet + d2.feet;
    sumOfDistances.inch = d1.inch + d2.inch;

    if(sumOfDistances.inch > 12) {
        ++sumOfDistances.feet;
        sumOfDistances.inch -= 12;
    } 

    cout << endl << "Sum of distances = " << sumOfDistances.feet << " feet  " << sumOfDistances.inch << " inches";
    return 0;
}
