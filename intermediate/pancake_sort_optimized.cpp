/*
 * Problem Description:
 * Implement Pancake Sort algorithm with some optimizations.
 * Given an unsorted array, sort the given array by only reversing sub-arrays (pancakes).
 */

#include <iostream>
#include <vector>
using namespace std;

void flip(vector<int>& arr, int i) {
    int start = 0;
    while (start < i) {
        swap(arr[start], arr[i]);
        start++;
        i--;
    }
}

int findMax(const vector<int>& arr, int n) {
    int mi = 0;
    for (int i = 0; i < n; ++i) {
        if (arr[i] > arr[mi])
            mi = i;
    }
    return mi;
}

void pancakeSort(vector<int>& arr) {
    int n = arr.size();
    for (int curr_size = n; curr_size > 1; --curr_size) {
        int mi = findMax(arr, curr_size);

        if (mi != curr_size - 1) {
            if (mi != 0) {
                flip(arr, mi);
            }
            flip(arr, curr_size - 1);
        }
    }
}

int main() {
    vector<int> arr = {23, 10, 20, 11, 12, 6, 7};
    cout << "Original array: \n";
    for(int x: arr) cout << x << " ";
    cout << "\n";

    pancakeSort(arr);

    cout << "Sorted array: \n";
    for(int x: arr) cout << x << " ";
    cout << "\n";

    return 0;
}
