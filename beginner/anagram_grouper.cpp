/*
 * Program Description:
 * Group Anagrams Algorithm in C++.
 * Given an array of strings, groups anagrams together.
 * An anagram is a word formed by rearranging the letters of a different word,
 * using all original letters exactly once.
 *
 * Approaches implemented:
 * 1. Sorted String Key: Sort each word alphabetically to form the canonical hash map key.
 *    Time Complexity: O(N * K log K) where N is number of words, K is max word length.
 * 2. Character Frequency Signature: Count letter occurrences (26-element array) to form key.
 *    Time Complexity: O(N * K) - optimal linear in total string length.
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

// Approach 1: Sorted string key
std::vector<std::vector<std::string>> groupAnagramsSortedKey(const std::vector<std::string>& words) {
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const std::string& word : words) {
        std::string key = word;
        std::sort(key.begin(), key.end());
        groups[key].push_back(word);
    }

    std::vector<std::vector<std::string>> result;
    result.reserve(groups.size());
    for (auto& pair : groups) {
        result.push_back(std::move(pair.second));
    }
    return result;
}

// Approach 2: Frequency count signature
std::vector<std::vector<std::string>> groupAnagramsCountKey(const std::vector<std::string>& words) {
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const std::string& word : words) {
        int count[26] = {0};
        for (char c : word) {
            count[c - 'a']++;
        }

        std::string key = "";
        for (int i = 0; i < 26; ++i) {
            if (count[i] > 0) {
                key += static_cast<char>('a' + i);
                key += std::to_string(count[i]);
            }
        }
        groups[key].push_back(word);
    }

    std::vector<std::vector<std::string>> result;
    result.reserve(groups.size());
    for (auto& pair : groups) {
        result.push_back(std::move(pair.second));
    }
    return result;
}

void printGroups(const std::vector<std::vector<std::string>>& groups) {
    for (size_t i = 0; i < groups.size(); ++i) {
        std::cout << "  Group " << (i + 1) << " (" << groups[i].size() << " words): [ ";
        for (size_t j = 0; j < groups[i].size(); ++j) {
            std::cout << "\"" << groups[i][j] << "\"" << (j + 1 < groups[i].size() ? ", " : " ");
        }
        std::cout << "]\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "         Group Anagrams Algorithm         \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> wordList = {
        "eat", "tea", "tan", "ate", "nat", "bat",
        "listen", "silent", "enlist", "google", "gogole"
    };

    std::cout << "Input Words (" << wordList.size() << "):\n  ";
    for (const auto& w : wordList) std::cout << w << " ";
    std::cout << "\n\n";

    std::cout << "--- Approach 1 (Sorted String Key) ---\n";
    auto res1 = groupAnagramsSortedKey(wordList);
    printGroups(res1);

    std::cout << "\n--- Approach 2 (Frequency Count Key) ---\n";
    auto res2 = groupAnagramsCountKey(wordList);
    printGroups(res2);

    return 0;
}
