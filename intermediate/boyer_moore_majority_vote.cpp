/*
 * Program Description:
 * Boyer-Moore Majority Vote Algorithm in C++.
 * Finds the majority element in an array in linear time O(N) and constant extra space O(1).
 *
 * Concepts & Algorithms:
 * 1. Majority Element (appears > N / 2 times):
 *    - Phase 1: Candidate pairing / elimination.
 *    - Phase 2: Verification pass to confirm count > N / 2.
 * 2. Generalized Majority Element (appears > N / 3 times):
 *    - At most 2 elements can appear more than N/3 times.
 *    - Maintains two candidate-counter pairs in O(1) space.
 *
 * Time Complexity: O(N)
 * Space Complexity: O(1)
 */

#include <iostream>
#include <vector>
#include <optional>

// Finds element appearing > N / 2 times using Boyer-Moore
std::optional<int> findMajorityElement(const std::vector<int>& nums) {
    if (nums.empty()) return std::nullopt;

    // Phase 1: Candidate selection
    int candidate = 0;
    int count = 0;

    for (int num : nums) {
        if (count == 0) {
            candidate = num;
            count = 1;
        } else if (num == candidate) {
            count++;
        } else {
            count--;
        }
    }

    // Phase 2: Verification pass
    int actualCount = 0;
    for (int num : nums) {
        if (num == candidate) {
            actualCount++;
        }
    }

    if (actualCount > static_cast<int>(nums.size()) / 2) {
        return candidate;
    }
    return std::nullopt;
}

// Finds elements appearing > N / 3 times (at most 2 elements)
std::vector<int> findMajorityElementsN3(const std::vector<int>& nums) {
    std::vector<int> result;
    if (nums.empty()) return result;

    int candidate1 = 0, candidate2 = 0;
    int count1 = 0, count2 = 0;

    // Phase 1: Find 2 potential candidates
    for (int num : nums) {
        if (num == candidate1) {
            count1++;
        } else if (num == candidate2) {
            count2++;
        } else if (count1 == 0) {
            candidate1 = num;
            count1 = 1;
        } else if (count2 == 0) {
            candidate2 = num;
            count2 = 1;
        } else {
            count1--;
            count2--;
        }
    }

    // Phase 2: Verify both candidates
    count1 = 0;
    count2 = 0;
    for (int num : nums) {
        if (num == candidate1) count1++;
        else if (num == candidate2) count2++;
    }

    int threshold = nums.size() / 3;
    if (count1 > threshold) result.push_back(candidate1);
    if (count2 > threshold) result.push_back(candidate2);

    return result;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Boyer-Moore Majority Vote Algorithm   \n";
    std::cout << "==========================================\n\n";

    // Test Case 1: Clear > N/2 majority
    std::vector<int> arr1 = {2, 2, 1, 1, 1, 2, 2};
    std::cout << "Array 1: ";
    for (int x : arr1) std::cout << x << " ";
    std::cout << "\n";
    auto maj1 = findMajorityElement(arr1);
    if (maj1) {
        std::cout << "  Majority (> N/2): " << *maj1 << "\n";
    } else {
        std::cout << "  No majority element found.\n";
    }

    // Test Case 2: No > N/2 majority
    std::vector<int> arr2 = {1, 2, 3, 4, 5, 2, 2};
    std::cout << "\nArray 2: ";
    for (int x : arr2) std::cout << x << " ";
    std::cout << "\n";
    auto maj2 = findMajorityElement(arr2);
    if (maj2) {
        std::cout << "  Majority (> N/2): " << *maj2 << "\n";
    } else {
        std::cout << "  No majority element found (verification failed).\n";
    }

    // Test Case 3: > N/3 candidates
    std::vector<int> arr3 = {1, 1, 1, 3, 3, 2, 2, 2};
    std::cout << "\nArray 3 (Testing > N/3): ";
    for (int x : arr3) std::cout << x << " ";
    std::cout << "\n";
    auto maj3 = findMajorityElementsN3(arr3);
    std::cout << "  Elements with freq > " << arr3.size() / 3 << ": ";
    for (int x : maj3) std::cout << x << " ";
    std::cout << "\n";

    return 0;
}
