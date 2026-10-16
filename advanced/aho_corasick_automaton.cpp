/*
 * Program Description:
 * Aho-Corasick Automaton for Multi-Pattern String Matching.
 * Locates all occurrences of a finite set of keywords (patterns) within an input text
 * simultaneously in linear time.
 *
 * Algorithm Structure:
 * 1. Trie Construction: Insert all keywords into a trie.
 * 2. Failure Link Construction (BFS):
 *    - Like the LPS array in KMP, but on a tree of prefixes.
 *    - A node's failure link points to the longest proper suffix of the string
 *      represented by that node that is also a prefix in the trie.
 * 3. Output / Dictionary Links:
 *    - Collects all matched pattern IDs reachable along failure chains.
 * 4. Text Traversal:
 *    - Feeds characters one-by-one, following transitions and failure links.
 *
 * Time Complexity:
 * - Build: O(total length of patterns * Alphabet)
 * - Search: O(|Text| + total matches)
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <map>

struct MatchResult {
    int position;       // Ending index in text
    int patternIndex;   // ID of pattern
    std::string pattern;// Pattern string
};

class AhoCorasick {
    static const int ALPHABET_SIZE = 26;

    struct Node {
        int children[ALPHABET_SIZE];
        int fail;
        std::vector<int> output; // Pattern IDs ending at this state

        Node() : fail(0) {
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                children[i] = 0;
            }
        }
    };

    std::vector<Node> trie;
    std::vector<std::string> patterns;

public:
    AhoCorasick() {
        trie.emplace_back(); // Root is state 0
    }

    void insertPattern(const std::string& pattern, int id) {
        int curr = 0;
        for (char c : pattern) {
            int idx = c - 'a';
            if (trie[curr].children[idx] == 0) {
                trie[curr].children[idx] = trie.size();
                trie.emplace_back();
            }
            curr = trie[curr].children[idx];
        }
        trie[curr].output.push_back(id);
    }

    void buildAutomaton(const std::vector<std::string>& keywordList) {
        patterns = keywordList;
        for (int i = 0; i < static_cast<int>(patterns.size()); ++i) {
            insertPattern(patterns[i], i);
        }

        std::queue<int> q;

        // Initialize depth 1 nodes: failure link goes to root (0)
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            int child = trie[0].children[i];
            if (child != 0) {
                trie[child].fail = 0;
                q.push(child);
            }
        }

        // BFS to build failure links for deeper nodes
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            // Merge output from failure state
            int failState = trie[u].fail;
            for (int outId : trie[failState].output) {
                trie[u].output.push_back(outId);
            }

            for (int c = 0; c < ALPHABET_SIZE; ++c) {
                int v = trie[u].children[c];
                if (v != 0) {
                    // Find failure link for child v
                    int f = trie[u].fail;
                    while (f != 0 && trie[f].children[c] == 0) {
                        f = trie[f].fail;
                    }
                    trie[v].fail = trie[f].children[c];
                    q.push(v);
                }
            }
        }
    }

    std::vector<MatchResult> search(const std::string& text) {
        std::vector<MatchResult> results;
        int curr = 0;

        for (int i = 0; i < static_cast<int>(text.length()); ++i) {
            char ch = text[i];
            if (ch < 'a' || ch > 'z') {
                curr = 0; // Reset state for characters outside alphabet
                continue;
            }
            int idx = ch - 'a';

            while (curr != 0 && trie[curr].children[idx] == 0) {
                curr = trie[curr].fail;
            }

            curr = trie[curr].children[idx];

            for (int patternId : trie[curr].output) {
                results.push_back({i, patternId, patterns[patternId]});
            }
        }

        return results;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "     Aho-Corasick Automaton Demo          \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> dictionary = {"he", "she", "his", "hers"};
    AhoCorasick ac;
    ac.buildAutomaton(dictionary);

    std::string text = "ahishers";
    std::cout << "Keywords : ";
    for (const auto& w : dictionary) std::cout << "\"" << w << "\" ";
    std::cout << "\nText     : \"" << text << "\"\n\n";

    std::vector<MatchResult> matches = ac.search(text);

    std::cout << "Matches Found (" << matches.size() << " occurrences):\n";
    for (const auto& m : matches) {
        int startPos = m.position - m.pattern.length() + 1;
        std::cout << "  Pattern \"" << m.pattern << "\" found at indices ["
                  << startPos << ".." << m.position << "]\n";
    }

    return 0;
}
