/*
 * Problem Description:
 * Write a program that finds all duplicate elements in an array. The array is taken
 * as input from the user and the program outputs all numbers that appear more than once.
 */

#include <iostream>
#include <vector>
#include <unordered_set>

int main() {
    int n;
    std::cout << "Enter number of elements: ";
    std::cin >> n;

    std::vector<int> arr(n);
    std::cout << "Enter elements: ";
    for(int i = 0; i < n; ++i)
        std::cin >> arr[i];

    std::unordered_set<int> seen, duplicates;
    for(int i = 0; i < n; ++i) {
        if(seen.find(arr[i]) != seen.end()) {
            duplicates.insert(arr[i]);
        } else {
            seen.insert(arr[i]);
        }
    }

    if(duplicates.empty()) {
        std::cout << "No duplicates found.\n";
    } else {
        std::cout << "Duplicates in array are: ";
        for(int dup : duplicates) {
            std::cout << dup << " ";
        }
        std::cout << "\n";
    }

    return 0;
}
