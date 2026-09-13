/*
 * Problem Description:
 * Implement Binary Search to find a specific element in a sorted array. Returns the index
 * of the element if found, otherwise returns -1.
 */

#include <iostream>
#include <vector>

int binarySearch(const std::vector<int>& arr, int l, int r, int x) {
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (arr[m] == x)
            return m;
        if (arr[m] < x)
            l = m + 1;
        else
            r = m - 1;
    }
    return -1;
}

int main() {
    int n;
    std::cout << "Enter number of sorted elements: ";
    std::cin >> n;

    std::vector<int> arr(n);
    std::cout << "Enter sorted elements: ";
    for(int i = 0; i < n; ++i)
        std::cin >> arr[i];

    int x;
    std::cout << "Enter element to search: ";
    std::cin >> x;

    int result = binarySearch(arr, 0, n - 1, x);
    if(result == -1)
        std::cout << "Element not found\n";
    else
        std::cout << "Element found at index " << result << "\n";

    return 0;
}
