/*
 * Program Description:
 * Auto-Complete System using Trie in C++.
 * Given a set of dictionary words with popularity weights (frequency),
 * answers prefix queries by returning top-K recommended word completions.
 *
 * Components:
 * 1. Trie node with children map, terminal flag, and frequency weight.
 * 2. Subtree DFS collection for all words sharing a queried prefix.
 * 3. Sorting by popularity descending, then lexicographical order.
 * 4. Top-K extraction in O(P + S log K) where P is prefix length and S is matched words.
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>

struct WordCandidate {
    std::string word;
    int frequency;

    bool operator<(const WordCandidate& other) const {
        if (frequency != other.frequency) {
            return frequency > other.frequency; // Highest frequency first
        }
        return word < other.word; // Lexicographical tie-breaker
    }
};

class AutocompleteTrie {
    struct TrieNode {
        std::unique_ptr<TrieNode> children[26];
        bool isEndOfWord = false;
        int frequency = 0;
    };

    std::unique_ptr<TrieNode> root;

    void collectWords(TrieNode* node, std::string& currentPrefix, std::vector<WordCandidate>& results) const {
        if (!node) return;

        if (node->isEndOfWord) {
            results.push_back({currentPrefix, node->frequency});
        }

        for (int i = 0; i < 26; ++i) {
            if (node->children[i]) {
                currentPrefix.push_back(static_cast<char>('a' + i));
                collectWords(node->children[i].get(), currentPrefix, results);
                currentPrefix.pop_back();
            }
        }
    }

public:
    AutocompleteTrie() : root(std::make_unique<TrieNode>()) {}

    void insert(const std::string& word, int frequency) {
        TrieNode* curr = root.get();
        for (char c : word) {
            int idx = c - 'a';
            if (!curr->children[idx]) {
                curr->children[idx] = std::make_unique<TrieNode>();
            }
            curr = curr->children[idx].get();
        }
        curr->isEndOfWord = true;
        curr->frequency = frequency;
    }

    std::vector<WordCandidate> getTopSuggestions(const std::string& prefix, int k) const {
        TrieNode* curr = root.get();
        for (char c : prefix) {
            int idx = c - 'a';
            if (!curr->children[idx]) {
                return {}; // Prefix not present
            }
            curr = curr->children[idx].get();
        }

        std::vector<WordCandidate> candidates;
        std::string path = prefix;
        collectWords(curr, path, candidates);

        std::sort(candidates.begin(), candidates.end());
        if (static_cast<int>(candidates.size()) > k) {
            candidates.resize(k);
        }
        return candidates;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Trie-Based Auto-Complete System      \n";
    std::cout << "==========================================\n\n";

    AutocompleteTrie trie;
    std::vector<std::pair<std::string, int>> dictionary = {
        {"code", 85}, {"coding", 95}, {"coder", 70},
        {"codepen", 60}, {"codec", 40}, {"coffee", 90},
        {"computer", 100}, {"computation", 75}, {"compile", 80}
    };

    for (const auto& [word, freq] : dictionary) {
        trie.insert(word, freq);
    }

    std::vector<std::string> searchPrefixes = {"cod", "co", "comp", "xyz"};

    for (const auto& prefix : searchPrefixes) {
        std::cout << "Search Prefix: \"" << prefix << "\" (Top 3 Suggestions):\n";
        auto suggestions = trie.getTopSuggestions(prefix, 3);
        if (suggestions.empty()) {
            std::cout << "  [No suggestions found]\n\n";
        } else {
            for (size_t i = 0; i < suggestions.size(); ++i) {
                std::cout << "  " << (i + 1) << ". " << suggestions[i].word
                          << " (Popularity Score: " << suggestions[i].frequency << ")\n";
            }
            std::cout << "\n";
        }
    }

    return 0;
}
