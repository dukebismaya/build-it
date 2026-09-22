/*
 * Problem Description:
 * Implement Comb Sort algorithm.
 * Comb Sort is mainly an improvement over Bubble Sort. Bubble sort always compares adjacent values. 
 * So all inversions are removed one by one. Comb Sort improves on Bubble Sort by using a gap of size more than 1.
 */

#include <iostream>
using namespace std;

int getNextGap(int gap) {
    gap = (gap * 10) / 13;
    if (gap < 1)
        return 1;
    return gap;
}

void combSort(int a[], int n) {
    int gap = n;
    bool swapped = true;
    
    while (gap != 1 || swapped == true) {
        gap = getNextGap(gap);
        swapped = false;
        
        for (int i = 0; i < n - gap; i++) {
            if (a[i] > a[i + gap]) {
                swap(a[i], a[i + gap]);
                swapped = true;
            }
        }
    }
}

int main() {
    int a[] = {8, 4, 1, 56, 3, -44, 23, -6, 28, 0};
    int n = sizeof(a) / sizeof(a[0]);
    
    cout << "Original array: \n";
    for (int i = 0; i < n; i++)
        cout << a[i] << " ";
    cout << "\n";
    
    combSort(a, n);
    
    cout << "Sorted array: \n";
    for (int i = 0; i < n; i++)
        cout << a[i] << " ";
    cout << "\n";
    
    return 0;
}
