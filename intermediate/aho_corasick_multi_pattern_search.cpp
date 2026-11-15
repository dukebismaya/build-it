/*
 * ============================================================================
 *  Aho-Corasick Multi-Pattern String Matching (Simplified Variant)
 * ============================================================================
 *
 *  Aho-Corasick is a finite-automaton-based algorithm that searches for
 *  multiple pattern strings simultaneously in a single pass over the text.
 *
 *  Construction:
 *  1) Build a trie (keyword tree) from all patterns.
 *  2) Compute failure links (suffix links) using BFS, analogous to the
 *     KMP failure function but generalized to a trie.
 *  3) Compute output links (dictionary suffix links) to chain through
 *     all patterns that end at each state.
 *
 *  Search:
 *  - Traverse the automaton one character at a time. At each state,
 *    follow output links to report all patterns ending at that position.
 *
 *  Time Complexity:
 *  - Build: O(sum of pattern lengths * alphabet size)
 *  - Search: O(text length + number of matches)
 *  Space Complexity: O(sum of pattern lengths * alphabet size)
 *
 *  Note: A more complete implementation exists in advanced/aho_corasick_automaton.cpp.
 *  This version focuses on clarity and educational presentation.
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <cstdint>
#include <iomanip>

namespace aho_corasick {

constexpr int ALPHABET = 26;

struct TrieNode {
    int children[ALPHABET];
    int failLink;
    int outputLink;
    int patternIndex; /* -1 if no pattern ends here */

    TrieNode() : failLink(0), outputLink(-1), patternIndex(-1) {
        for (int i = 0; i < ALPHABET; ++i) {
            children[i] = -1;
        }
    }
};

class AhoCorasick {
public:
    AhoCorasick() {
        nodes.emplace_back(); /* Root node */
    }

    void addPattern(const std::string& pattern, int index) {
        int cur = 0;
        for (char ch : pattern) {
            int c = ch - 'a';
            if (nodes[cur].children[c] == -1) {
                nodes[cur].children[c] = static_cast<int>(nodes.size());
                nodes.emplace_back();
            }
            cur = nodes[cur].children[c];
        }
        nodes[cur].patternIndex = index;
    }

    void build() {
        std::queue<int> bfsQueue;

        /* Initialize root's children: missing transitions loop to root */
        for (int c = 0; c < ALPHABET; ++c) {
            if (nodes[0].children[c] == -1) {
                nodes[0].children[c] = 0; /* Self-loop */
            } else {
                nodes[nodes[0].children[c]].failLink = 0;
                bfsQueue.push(nodes[0].children[c]);
            }
        }

        /* BFS to compute fail links and output links */
        while (!bfsQueue.empty()) {
            int u = bfsQueue.front();
            bfsQueue.pop();

            for (int c = 0; c < ALPHABET; ++c) {
                int v = nodes[u].children[c];
                if (v == -1) {
                    /* Fill missing transitions using fail link */
                    nodes[u].children[c] = nodes[nodes[u].failLink].children[c];
                } else {
                    nodes[v].failLink = nodes[nodes[u].failLink].children[c];

                    /* Output link: nearest ancestor with a pattern ending */
                    if (nodes[nodes[v].failLink].patternIndex != -1) {
                        nodes[v].outputLink = nodes[v].failLink;
                    } else {
                        nodes[v].outputLink = nodes[nodes[v].failLink].outputLink;
                    }

                    bfsQueue.push(v);
                }
            }
        }
    }

    struct Match {
        int position; /* Ending position in text (inclusive) */
        int patternIndex;
    };

    std::vector<Match> search(const std::string& text) const {
        std::vector<Match> matches;
        int state = 0;

        for (int i = 0; i < static_cast<int>(text.size()); ++i) {
            int c = text[i] - 'a';
            state = nodes[state].children[c];

            /* Check current state and follow output links */
            int temp = state;
            while (temp != -1 && temp != 0) {
                if (nodes[temp].patternIndex != -1) {
                    matches.push_back({i, nodes[temp].patternIndex});
                }
                temp = nodes[temp].outputLink;
            }
            /* Also check root (state 0) */
            if (temp == 0 && nodes[0].patternIndex != -1) {
                matches.push_back({i, nodes[0].patternIndex});
            }
        }

        return matches;
    }

    int stateCount() const { return static_cast<int>(nodes.size()); }

private:
    std::vector<TrieNode> nodes;
};

} // namespace aho_corasick

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Aho-Corasick Multi-Pattern String Matching\n";
    std::cout << "========================================================\n\n";

    std::vector<std::string> patterns = {"he", "she", "his", "hers", "her"};
    std::string text = "ahishersheishis";

    aho_corasick::AhoCorasick ac;
    for (int i = 0; i < static_cast<int>(patterns.size()); ++i) {
        ac.addPattern(patterns[i], i);
    }
    ac.build();

    std::cout << "Patterns: {";
    for (size_t i = 0; i < patterns.size(); ++i) {
        std::cout << "\"" << patterns[i] << "\"";
        if (i + 1 < patterns.size()) std::cout << ", ";
    }
    std::cout << "}\n";
    std::cout << "Text: \"" << text << "\"\n";
    std::cout << "Automaton states: " << ac.stateCount() << "\n\n";

    auto matches = ac.search(text);

    std::cout << "Matches found: " << matches.size() << "\n";
    std::cout << std::setw(12) << "Pattern" << std::setw(10) << "End Pos"
              << std::setw(12) << "Start Pos" << std::setw(16) << "Substring" << "\n";
    std::cout << "----------------------------------------------------\n";

    for (auto& m : matches) {
        int startPos = m.position - static_cast<int>(patterns[m.patternIndex].size()) + 1;
        std::string sub = text.substr(startPos, patterns[m.patternIndex].size());

        std::cout << std::setw(12) << ("\"" + patterns[m.patternIndex] + "\"")
                  << std::setw(10) << m.position
                  << std::setw(12) << startPos
                  << std::setw(16) << ("\"" + sub + "\"") << "\n";
    }

    /* Test with another text */
    std::cout << "\n--- Second Test ---\n";
    std::vector<std::string> patterns2 = {"cat", "at", "bat", "cab"};
    std::string text2 = "acatabcatbat";

    aho_corasick::AhoCorasick ac2;
    for (int i = 0; i < static_cast<int>(patterns2.size()); ++i) {
        ac2.addPattern(patterns2[i], i);
    }
    ac2.build();

    std::cout << "Patterns: {";
    for (size_t i = 0; i < patterns2.size(); ++i) {
        std::cout << "\"" << patterns2[i] << "\"";
        if (i + 1 < patterns2.size()) std::cout << ", ";
    }
    std::cout << "}\n";
    std::cout << "Text: \"" << text2 << "\"\n\n";

    auto matches2 = ac2.search(text2);
    std::cout << "Matches found: " << matches2.size() << "\n";
    for (auto& m : matches2) {
        int startPos = m.position - static_cast<int>(patterns2[m.patternIndex].size()) + 1;
        std::cout << "  \"" << patterns2[m.patternIndex] << "\" at position " << startPos << "\n";
    }

    return 0;
}
