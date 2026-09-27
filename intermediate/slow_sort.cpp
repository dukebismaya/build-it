/*
 * Problem Description:
 * Implement Slow Sort algorithm.
 * Slowsort is a humorous sorting algorithm based on the principle of multiply and surrender,
 * a parody of divide and conquer.
 */

#include <iostream>
#include <vector>

using namespace std;

void slowSort(vector<int>& arr, int i, int j) {
    if (i >= j) return;
    
    int m = (i + j) / 2;
    slowSort(arr, i, m);
    slowSort(arr, m + 1, j);
    
    if (arr[m] > arr[j]) {
        swap(arr[m], arr[j]);
    }
    
    slowSort(arr, i, j - 1);
}

int main() {
    vector<int> arr = {6, 2, 8, 3, 5, 1};
    cout << "Original array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    slowSort(arr, 0, arr.size() - 1);
    
    cout << "Sorted array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    return 0;
}
