/*
 * Problem Description:
 * Implement Borwein's Algorithm for calculating pi conceptually.
 * The Borwein's algorithm is a class of algorithms developed by Jonathan and Peter Borwein to calculate the value of 1/pi.
 */

#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

void borweinPi(int iterations) {
    // Conceptual demonstration of Borwein's quadratically convergent algorithm.
    // For many digits, an arbitrary-precision library is required.
    double a = sqrt(2);
    double b = 0;
    double p = 2 + sqrt(2);

    for (int i = 0; i < iterations; i++) {
        double a_next = (sqrt(a) + 1.0 / sqrt(a)) / 2.0;
        double b_next = (1.0 + b) * sqrt(a) / (a + b);
        double p_next = p * b_next * (1.0 + a_next) / (1.0 + b_next);

        a = a_next;
        b = b_next;
        p = p_next;
    }

    cout << setprecision(15) << "Calculated Pi approx: " << p << "\n";
    cout << "Real Pi approx:       3.141592653589793\n";
}

int main() {
    cout << "Borwein's Algorithm (Conceptual with standard double precision):\n";
    borweinPi(5); 
    return 0;
}
