/*
 * Problem Description:
 * Implement Stooge Sort algorithm.
 * Stooge sort is a recursive sorting algorithm with a time complexity of O(n^(log 3 / log 1.5)) = O(n^2.7095...).
 */

#include <iostream>
using namespace std;

void stoogeSort(int arr[], int l, int h) {
    if (l >= h)
        return;
        
    if (arr[l] > arr[h])
        swap(arr[l], arr[h]);
        
    if (h - l + 1 > 2) {
        int t = (h - l + 1) / 3;
        
        stoogeSort(arr, l, h - t);
        stoogeSort(arr, l + t, h);
        stoogeSort(arr, l, h - t);
    }
}

int main() {
    int arr[] = {2, 4, 5, 3, 1};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    stoogeSort(arr, 0, n - 1);
    
    cout << "Sorted array : \n";
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << "\n";
    
    return 0;
}
