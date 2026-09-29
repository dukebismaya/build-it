/*
 * Problem Description:
 * Implement Bead Sort (Gravity Sort) algorithm.
 * It is a natural sorting algorithm, developed by Joshua J. Arulanandham, 
 * Cristian S. Calude and Michael J. Dinneen in 2002.
 */

#include <iostream>
#include <vector>

using namespace std;

void beadSort(vector<int>& a) {
    int n = a.size();
    if (n == 0) return;

    int max = a[0];
    for (int i = 1; i < n; i++)
        if (a[i] > max)
            max = a[i];

    vector<vector<unsigned char>> beads(n, vector<unsigned char>(max, 0));

    for (int i = 0; i < n; i++)
        for (int j = 0; j < a[i]; j++)
            beads[i][j] = 1;

    for (int j = 0; j < max; j++) {
        int sum = 0;
        for (int i = 0; i < n; i++) {
            sum += beads[i][j];
            beads[i][j] = 0;
        }
        for (int i = n - sum; i < n; i++)
            beads[i][j] = 1;
    }

    for (int i = 0; i < n; i++) {
        int j;
        for (j = 0; j < max && beads[i][j]; j++);
        a[i] = j;
    }
}

int main() {
    vector<int> arr = {5, 3, 1, 7, 4, 1, 1, 20};
    
    cout << "Original array: \n";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    
    beadSort(arr);
    
    cout << "Sorted array: \n";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    
    return 0;
}
