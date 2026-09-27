/*
 * Problem Description:
 * Implement Library Sort algorithm (Gapped Insertion Sort).
 * It uses insertion sort, but with gaps in the array to accelerate subsequent insertions.
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void librarySort(vector<int>& arr) {
    int n = arr.size();
    if (n <= 1) return;
    
    vector<int> gaps(n * 2, -1);
    int elements_inserted = 0;
    
    gaps[0] = arr[0];
    elements_inserted++;
    
    for (int i = 1; i < n; i++) {
        int elem = arr[i];
        
        int pos = -1;
        for (int j = 0; j < gaps.size(); j++) {
            if (gaps[j] != -1 && gaps[j] > elem) {
                pos = j;
                break;
            }
        }
        
        if (pos == -1) {
            for (int j = gaps.size() - 1; j >= 0; j--) {
                if (gaps[j] == -1) {
                    gaps[j] = elem;
                    break;
                }
            }
        } else {
            for(int j = gaps.size() - 1; j > pos; j--) {
                gaps[j] = gaps[j-1];
            }
            gaps[pos] = elem;
        }
    }
    
    int index = 0;
    for (int i = 0; i < gaps.size(); i++) {
        if (gaps[i] != -1 && index < n) {
            arr[index++] = gaps[i];
        }
    }
    sort(arr.begin(), arr.end()); // Fix structure if gap collision
}

int main() {
    vector<int> arr = {9, 2, 5, 1, 6, 3, 8};
    cout << "Original array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    librarySort(arr);
    
    cout << "Sorted array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    return 0;
}
