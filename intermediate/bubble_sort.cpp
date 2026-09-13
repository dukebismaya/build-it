/*
 * Problem Description:
 * Implement the Bubble Sort algorithm to sort an array of integers in ascending order.
 * The program should prompt the user for the number of elements, take the elements as input,
 * and print the sorted array.
 */

#include <iostream>
#include <vector>

void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    for(int i = 0; i < n-1; ++i) {
        for(int j = 0; j < n-i-1; ++j) {
            if(arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
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

    bubbleSort(arr);

    std::cout << "Sorted array: ";
    for(int i = 0; i < n; ++i)
        std::cout << arr[i] << " ";
    std::cout << "\n";

    return 0;
}
