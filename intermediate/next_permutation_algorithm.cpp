/*
 * Program Description:
 * Next Lexicographical Permutation Algorithm (Narayana Pandita's Algorithm).
 * Re-arranges numbers into the lexicographically next greater permutation.
 * This is the exact algorithm underlying std::next_permutation in the C++ STL.
 *
 * Four-Step Algorithm:
 * 1. Find largest index i such that arr[i] < arr[i + 1].
 *    (If no such index exists, the permutation is the last/descending one;
 *     reverse to smallest and return false).
 * 2. Find largest index j > i such that arr[j] > arr[i].
 * 3. Swap arr[i] and arr[j].
 * 4. Reverse the sub-array from i + 1 to the end of the array.
 *
 * Time Complexity: O(N) per step.
 * Space Complexity: O(1) in-place.
 */

#include <iostream>
#include <vector>
#include <algorithm>

// Custom implementation of next_permutation
bool customNextPermutation(std::vector<int>& arr) {
    int n = arr.size();
    if (n <= 1) return false;

    // Step 1: Find largest index i such that arr[i] < arr[i + 1]
    int i = n - 2;
    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }

    // If no such index, the sequence is in descending order
    if (i < 0) {
        std::reverse(arr.begin(), arr.end());
        return false;
    }

    // Step 2: Find largest index j > i such that arr[j] > arr[i]
    int j = n - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }

    // Step 3: Swap elements at i and j
    std::swap(arr[i], arr[j]);

    // Step 4: Reverse the suffix starting at i + 1
    std::reverse(arr.begin() + i + 1, arr.end());

    return true;
}

void printVector(const std::vector<int>& v) {
    std::cout << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        std::cout << v[i] << (i + 1 < v.size() ? ", " : "");
    }
    std::cout << "]\n";
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "   Next Permutation Algorithm (In-Place)  \n";
    std::cout << "==========================================\n\n";

    // Demo 1: Step-by-step next permutation transitions
    std::vector<int> test = {1, 2, 3};
    std::cout << "Generating all permutations of [1, 2, 3]:\n";
    int count = 1;
    std::cout << "  #" << count++ << ": ";
    printVector(test);

    while (customNextPermutation(test)) {
        std::cout << "  #" << count++ << ": ";
        printVector(test);
    }

    std::cout << "\nDemo 2: Specific test cases:\n";
    std::vector<std::vector<int>> cases = {
        {1, 5, 8, 4, 7, 6, 5, 3, 1},
        {3, 2, 1},
        {1, 1, 5}
    };

    for (auto c : cases) {
        std::cout << "Original : ";
        printVector(c);

        std::vector<int> stdCopy = c;
        bool customHasNext = customNextPermutation(c);
        bool stdHasNext = std::next_permutation(stdCopy.begin(), stdCopy.end());

        std::cout << "Next     : ";
        printVector(c);
        std::cout << "STL Match: " << (c == stdCopy && customHasNext == stdHasNext ? "PERFECT [PASS]" : "MISMATCH") << "\n\n";
    }

    return 0;
}
