/*
 * Problem Description:
 * Implement Introsort algorithm.
 * Introsort or introspective sort is a hybrid sorting algorithm that provides both fast average performance and (asymptotically) optimal worst-case performance.
 * It begins with quicksort, it switches to heapsort when the recursion depth exceeds a level based on the number of elements being sorted.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

void insertionSort(vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int partition(vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void introSortUtil(vector<int>& arr, int begin, int end, int depthLimit) {
    int size = end - begin;
    if (size < 16) {
        insertionSort(arr, begin, end);
        return;
    }
    if (depthLimit == 0) {
        make_heap(arr.begin() + begin, arr.begin() + end + 1);
        sort_heap(arr.begin() + begin, arr.begin() + end + 1);
        return;
    }
    int pivot = partition(arr, begin, end);
    introSortUtil(arr, begin, pivot - 1, depthLimit - 1);
    introSortUtil(arr, pivot + 1, end, depthLimit - 1);
}

void introSort(vector<int>& arr) {
    int depthLimit = 2 * log(arr.size());
    introSortUtil(arr, 0, arr.size() - 1, depthLimit);
}

int main() {
    vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    cout << "Original Array: ";
    for(int x: arr) cout << x << " ";
    cout << "\n";

    introSort(arr);

    cout << "Sorted Array: ";
    for(int x: arr) cout << x << " ";
    cout << "\n";

    return 0;
}
