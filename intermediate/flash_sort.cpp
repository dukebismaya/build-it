/*
 * Problem Description:
 * Implement Flash Sort algorithm.
 * Flash sort is a distribution sorting algorithm showing linear time complexity O(n) for uniformly distributed data sets.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void flashSort(vector<int>& arr) {
    int n = arr.size();
    if (n == 0) return;
    int m = 0.45 * n;
    vector<int> l(m, 0);
    
    int min_val = arr[0], max_val = arr[0], max_idx = 0;
    for (int i = 1; i < n; i++) {
        if (arr[i] < min_val) min_val = arr[i];
        if (arr[i] > max_val) {
            max_val = arr[i];
            max_idx = i;
        }
    }
    if (min_val == max_val) return;
    
    double c1 = (double)(m - 1) / (max_val - min_val);
    
    for (int i = 0; i < n; i++) {
        int k = c1 * (arr[i] - min_val);
        l[k]++;
    }
    for (int i = 1; i < m; i++) {
        l[i] += l[i - 1];
    }
    
    swap(arr[max_idx], arr[0]);
    
    int nmove = 0;
    int j = 0;
    int k = m - 1;
    
    while (nmove < n - 1) {
        while (j > l[k] - 1) {
            j++;
            k = c1 * (arr[j] - min_val);
        }
        int flash = arr[j];
        while (j != l[k]) {
            k = c1 * (flash - min_val);
            l[k]--;
            int hold = arr[l[k]];
            arr[l[k]] = flash;
            flash = hold;
            nmove++;
        }
    }
    
    // Insertion sort for final pass
    for (int i = 1; i < n; i++) {
        int hold = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > hold) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = hold;
    }
}

int main() {
    vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    cout << "Original Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    flashSort(arr);
    
    cout << "Sorted Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    return 0;
}
