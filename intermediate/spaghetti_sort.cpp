/*
 * Problem Description:
 * Implement Spaghetti Sort algorithm conceptually.
 * Spaghetti sort is a linear-time sorting algorithm for sorting a list of items 
 * having continuous one-dimensional properties.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void spaghettiSort(vector<int>& arr) {
    if (arr.empty()) return;
    
    int max_val = *max_element(arr.begin(), arr.end());
    
    vector<int> sorted_arr;
    
    for (int i = 1; i <= max_val; i++) {
        for (int x : arr) {
            if (x == i) {
                sorted_arr.push_back(x);
            }
        }
    }
    arr = sorted_arr;
}

int main() {
    vector<int> arr = {4, 1, 3, 2, 8, 5, 9, 7, 6};
    cout << "Original array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    spaghettiSort(arr);
    
    cout << "Sorted array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    return 0;
}
