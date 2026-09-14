/*
 * Problem Description:
 * Merge two sorted arrays into a single sorted array. The program takes two sorted arrays
 * as input and produces a third sorted array containing all elements from both.
 */

#include <iostream>
#include <vector>

std::vector<int> mergeArrays(const std::vector<int>& arr1, const std::vector<int>& arr2) {
    std::vector<int> result;
    int i = 0, j = 0;
    
    while (i < arr1.size() && j < arr2.size()) {
        if (arr1[i] < arr2[j])
            result.push_back(arr1[i++]);
        else
            result.push_back(arr2[j++]);
    }
    
    while (i < arr1.size())
        result.push_back(arr1[i++]);
        
    while (j < arr2.size())
        result.push_back(arr2[j++]);
        
    return result;
}

int main() {
    int n1, n2;
    std::cout << "Enter size of first sorted array: ";
    std::cin >> n1;
    std::vector<int> arr1(n1);
    std::cout << "Enter elements: ";
    for(int i=0; i<n1; i++) std::cin >> arr1[i];
    
    std::cout << "Enter size of second sorted array: ";
    std::cin >> n2;
    std::vector<int> arr2(n2);
    std::cout << "Enter elements: ";
    for(int i=0; i<n2; i++) std::cin >> arr2[i];

    std::vector<int> merged = mergeArrays(arr1, arr2);

    std::cout << "Merged sorted array: ";
    for(int val : merged)
        std::cout << val << " ";
    std::cout << "\n";

    return 0;
}
