/*
 * Problem Description:
 * Implement Pigeonhole Sort algorithm.
 * Pigeonhole sorting is a sorting algorithm that is suitable for sorting lists of elements 
 * where the number of elements and the number of possible key values are approximately the same.
 */

#include <bits/stdc++.h>
using namespace std;

void pigeonholeSort(vector<int>& arr) {
    int n = arr.size();
    if (n == 0) return;

    int min = arr[0], max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] < min)
            min = arr[i];
        if (arr[i] > max)
            max = arr[i];
    }
    int range = max - min + 1;
    
    vector<int> holes[range];
    
    for (int i = 0; i < n; i++)
        holes[arr[i] - min].push_back(arr[i]);
        
    int index = 0; 
    for (int i = 0; i < range; i++) {
        vector<int>::iterator it;
        for (it = holes[i].begin(); it != holes[i].end(); ++it)
            arr[index++] = *it;
    }
}

int main() {
    vector<int> arr = {8, 3, 2, 7, 4, 6, 8};
    pigeonholeSort(arr);
    
    cout << "Sorted order is : \n";
    for (int i = 0; i < arr.size(); i++)
        cout << arr[i] << " ";
    cout << "\n";
    
    return 0;
}
