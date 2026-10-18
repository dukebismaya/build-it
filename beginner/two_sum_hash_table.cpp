/*
 * Program Description:
 * Two Sum Problem Solver in C++.
 * Given an array of integers and a target sum, find two numbers such that
 * their sum equals the target.
 *
 * Approaches:
 * 1. Hash Map (One-Pass):
 *    For each number x, checks if (target - x) is already in the map.
 *    If found, returns the indices. If not, records x with its index.
 *    Time Complexity: O(N)
 *    Space Complexity: O(N)
 * 2. All Unique Pairs Finder:
 *    Finds all distinct pairs of numbers that sum to the target.
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>

// Finds the first pair of indices whose values add up to target
std::optional<std::pair<int, int>> twoSumIndices(const std::vector<int>& nums, int target) {
    std::unordered_map<int, int> seen; // value -> index

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        int complement = target - nums[i];
        if (seen.find(complement) != seen.end()) {
            return std::make_pair(seen[complement], i);
        }
        seen[nums[i]] = i;
    }
    return std::nullopt;
}

// Finds all unique pairs of values (a, b) such that a + b == target
std::vector<std::pair<int, int>> allUniquePairs(const std::vector<int>& nums, int target) {
    std::unordered_set<int> seen;
    std::unordered_set<int> usedPairs;
    std::vector<std::pair<int, int>> pairs;

    for (int num : nums) {
        int complement = target - num;
        if (seen.find(complement) != seen.end()) {
            int smaller = std::min(num, complement);
            int larger = std::max(num, complement);
            if (usedPairs.find(smaller) == usedPairs.end()) {
                pairs.push_back({smaller, larger});
                usedPairs.insert(smaller);
            }
        }
        seen.insert(num);
    }

    return pairs;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Two Sum Problem (Hash Map)        \n";
    std::cout << "==========================================\n\n";

    std::vector<int> nums = {2, 7, 11, 15, 3, 6, 4, 5, 2};
    int target = 9;

    std::cout << "Input Array: ";
    for (int x : nums) std::cout << x << " ";
    std::cout << "\nTarget Sum : " << target << "\n\n";

    // Test 1: Single pair of indices
    auto indices = twoSumIndices(nums, target);
    if (indices) {
        std::cout << "First Pair Found at indices: [" << indices->first << ", " << indices->second << "]\n";
        std::cout << "Values: " << nums[indices->first] << " + " << nums[indices->second]
                  << " = " << target << "\n\n";
    } else {
        std::cout << "No two sum solution exists.\n\n";
    }

    // Test 2: All unique pairs
    auto pairs = allUniquePairs(nums, target);
    std::cout << "All Unique Value Pairs Summing to " << target << ":\n";
    for (const auto& [a, b] : pairs) {
        std::cout << "  (" << a << ", " << b << ") -> " << a + b << "\n";
    }

    return 0;
}
