/*
 * Problem Description:
 * Implement Gnome Sort algorithm.
 * Gnome Sort is based on the technique used by the standard Dutch Garden Gnome.
 * Here is how a garden gnome sorts a line of flower pots.
 */

#include <iostream>
using namespace std;

void gnomeSort(int arr[], int n) {
    int index = 0;
    
    while (index < n) {
        if (index == 0)
            index++;
        if (arr[index] >= arr[index - 1])
            index++;
        else {
            swap(arr[index], arr[index - 1]);
            index--;
        }
    }
    return;
}

int main() {
    int arr[] = {34, 2, 10, -9};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    gnomeSort(arr, n);
    
    cout << "Sorted sequence after Gnome sort: \n";
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << "\n";
    
    return 0;
}
