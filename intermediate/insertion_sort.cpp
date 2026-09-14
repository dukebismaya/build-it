/*
 * Problem Description:
 * Implement the Insertion Sort algorithm to sort an array of integers. Insertion sort works
 * similarly to the way you sort playing cards in your hands. The array is virtually split 
 * into a sorted and an unsorted part.
 */

#include <iostream>
#include <vector>

void insertionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

int main() {
    int n;
    std::cout << "Enter number of elements: ";
    std::cin >> n;

    std::vector<int> arr(n);
    std::cout << "Enter elements: ";
    for(int i = 0; i < n; ++i)
        std::cin >> arr[i];

    insertionSort(arr);

    std::cout << "Sorted array: ";
    for(int i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << "\n";

    return 0;
}
