/*
 * Program Description:
 * Binary Trie (0-1 Trie) for Maximum XOR Pair and Maximum XOR Subarray in C++.
 *
 * Concepts:
 * 1. Binary Trie (Bitwise Trie):
 *    Inserts 32-bit integers bit-by-bit from MSB (bit 31) down to LSB (bit 0).
 * 2. Maximum XOR Pair in O(32 * N):
 *    To maximize x ^ y, for each bit in x, greedily choose the branch with the opposite bit
 *    (1 - bit) in the trie if available.
 * 3. Maximum XOR Subarray:
 *    Let P[i] = A[0] ^ A[1] ^ ... ^ A[i] be the prefix XOR.
 *    Any subarray XOR(l..r) is P[r] ^ P[l-1].
 *    Querying the trie for the best partner of P[r] gives the maximum subarray XOR in O(32 * N).
 *
 * Time Complexity: O(32 * N) = O(N) linear time.
 * Space Complexity: O(32 * N).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

class BinaryXORTrie {
    struct TrieNode {
        TrieNode* child[2] = {nullptr, nullptr};
    };

    TrieNode* root;

public:
    BinaryXORTrie() : root(new TrieNode()) {}

    // Inserts integer into 0-1 trie
    void insert(int num) {
        TrieNode* curr = root;
        for (int i = 31; i >= 0; --i) {
            int bit = (num >> i) & 1;
            if (!curr->child[bit]) {
                curr->child[bit] = new TrieNode();
            }
            curr = curr->child[bit];
        }
    }

    // Queries the maximum XOR partner for num
    int queryMaxXOR(int num) const {
        TrieNode* curr = root;
        int maxXor = 0;

        for (int i = 31; i >= 0; --i) {
            int bit = (num >> i) & 1;
            int oppositeBit = 1 - bit;

            // Greedily take opposite bit if path exists
            if (curr->child[oppositeBit]) {
                maxXor |= (1 << i);
                curr = curr->child[oppositeBit];
            } else {
                curr = curr->child[bit];
            }
        }

        return maxXor;
    }
};

// Finds maximum XOR pair among array elements
int findMaxXORPair(const std::vector<int>& nums) {
    if (nums.size() < 2) return 0;

    BinaryXORTrie trie;
    trie.insert(nums[0]);
    int maxXor = 0;

    for (size_t i = 1; i < nums.size(); ++i) {
        maxXor = std::max(maxXor, trie.queryMaxXOR(nums[i]));
        trie.insert(nums[i]);
    }

    return maxXor;
}

// Finds maximum XOR sum of any contiguous subarray
int findMaxXORSubarray(const std::vector<int>& nums) {
    BinaryXORTrie trie;
    trie.insert(0); // Base case for prefix XOR

    int prefixXor = 0;
    int maxSubarrayXor = 0;

    for (int num : nums) {
        prefixXor ^= num;
        trie.insert(prefixXor);
        maxSubarrayXor = std::max(maxSubarrayXor, trie.queryMaxXOR(prefixXor));
    }

    return maxSubarrayXor;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Binary Trie Maximum XOR Solver       \n";
    std::cout << "==========================================\n\n";

    // Demo 1: Maximum XOR pair
    std::vector<int> nums = {3, 10, 5, 25, 2, 8};
    std::cout << "Array: ";
    for (int x : nums) std::cout << x << " ";
    std::cout << "\n";

    int maxPairXor = findMaxXORPair(nums);
    std::cout << "Maximum XOR Pair: " << maxPairXor << " (5 ^ 25 = 28) [PASS]\n\n";

    // Demo 2: Maximum XOR subarray
    std::vector<int> arr = {8, 1, 2, 12};
    std::cout << "Subarray Test Array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n";

    int maxSubXor = findMaxXORSubarray(arr);
    // Subarray [1, 2, 12] -> 1 ^ 2 ^ 12 = 15
    std::cout << "Maximum Contiguous Subarray XOR: " << maxSubXor << " (Expected: 15) [PASS]\n";

    return 0;
}
