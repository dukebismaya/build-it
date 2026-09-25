/*
 * Problem Description:
 * Implement Bingo Sort algorithm.
 * Bingo sort is a variant of Selection sort. The algorithm looks through the remaining items 
 * to find the greatest value and moves all items with that value to their final location.
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void bingoSort(vector<int>& arr) {
    int n = arr.size();
    if (n == 0) return;
    
    int maxVal = arr[0], minVal = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > maxVal) maxVal = arr[i];
        if (arr[i] < minVal) minVal = arr[i];
    }
    
    int largest = maxVal;
    int nextLargest = minVal;
    int nextInsert = n - 1;
    
    while (largest > minVal) {
        for (int i = nextInsert; i >= 0; i--) {
            if (arr[i] == largest) {
                swap(arr[i], arr[nextInsert]);
                nextInsert--;
            } else if (arr[i] < largest && arr[i] > nextLargest) {
                nextLargest = arr[i];
            }
        }
        largest = nextLargest;
        nextLargest = minVal;
    }
}

int main() {
    vector<int> arr = {5, 4, 8, 5, 4, 8, 5, 4, 8, 4};
    
    cout << "Original array: \n";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    
    bingoSort(arr);
    
    cout << "Sorted array: \n";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    return 0;
}
