/*
 * Problem Description:
 * Implement the Quick Sort algorithm. It picks an element as pivot and partitions the 
 * given array around the picked pivot, placing smaller elements to the left and larger to the right.
 */

#include <iostream>
#include <vector>

int partition(std::vector<int>& arr, int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSort(std::vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

int main() {
    int n;
    std::cout << "Enter number of elements: ";
    std::cin >> n;
    
    std::vector<int> arr(n);
    std::cout << "Enter elements: ";
    for(int i = 0; i < n; i++)
        std::cin >> arr[i];
        
    quickSort(arr, 0, n - 1);
    
    std::cout << "Sorted array: ";
    for(int i = 0; i < n; i++)
        std::cout << arr[i] << " ";
    std::cout << "\n";
    
    return 0;
}
