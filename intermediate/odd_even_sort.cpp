/*
 * Problem Description:
 * Implement Odd-Even Sort (Brick Sort).
 * This is basically a variation of bubble-sort. This algorithm is divided into two phases- 
 * Odd and Even Phase. The algorithm runs until the array elements are sorted and in each iteration 
 * two phases occurs- Odd and Even Phases.
 */

#include <iostream>
using namespace std;

void oddEvenSort(int arr[], int n) {
    bool isSorted = false;
    
    while (!isSorted) {
        isSorted = true;
        for (int i = 1; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
        for (int i = 0; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i + 1]);
                isSorted = false;
            }
        }
    }
    return;
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << "\n";
}

int main() {
    int arr[] = {34, 2, 10, -9};
    int n = sizeof(arr)/sizeof(arr[0]);
    
    cout << "Original array: \n";
    printArray(arr, n);
    
    oddEvenSort(arr, n);
    
    cout << "Sorted array: \n";
    printArray(arr, n);
    return 0;
}
