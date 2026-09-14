/*
 * Problem Description:
 * Implement Linear Search to find a specific element in an array. The program 
 * iterates through each element and checks if it matches the target element.
 */

#include <iostream>
#include <vector>

int linearSearch(const std::vector<int>& arr, int x) {
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] == x)
            return i;
    }
    return -1;
}

int main() {
    int n;
    std::cout << "Enter number of elements: ";
    std::cin >> n;

    std::vector<int> arr(n);
    std::cout << "Enter elements: ";
    for(int i = 0; i < n; ++i)
        std::cin >> arr[i];

    int x;
    std::cout << "Enter element to search: ";
    std::cin >> x;

    int result = linearSearch(arr, x);
    if(result == -1)
        std::cout << "Element not found\n";
    else
        std::cout << "Element found at index " << result << "\n";

    return 0;
}
