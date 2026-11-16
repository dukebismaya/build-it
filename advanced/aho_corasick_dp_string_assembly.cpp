/*
 * ============================================================================
 *  Aho-Corasick + DP: String Assembly from Dictionary Pieces
 * ============================================================================
 *
 *  Problem: Given a target string T and a dictionary of K pattern strings,
 *  find the minimum number of dictionary pieces needed to construct T by
 *  concatenation (pieces may overlap in T but each segment must exactly match
 *  a dictionary word). If T cannot be constructed, report -1.
 *
 *  Approach:
 *  1) Build an Aho-Corasick automaton from the dictionary.
 *  2) For each position i in T, find all dictionary words ending at i
 *     (via the automaton's output/dictionary suffix links).
 *  3) Run a DP: dp[i] = minimum pieces to form T[0..i-1].
 *     For each word w ending at position i-1 with length L:
 *       dp[i] = min(dp[i], dp[i - L] + 1)
 *
 *  Time Complexity:  O(|T| * ALPHABET + sum(|patterns|) + matches)
 *  Space Complexity: O(sum(|patterns|) * ALPHABET + |T|)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <climits>

namespace aho_dp {

constexpr int ALPHA = 26;

struct TrieNode {
    int children[ALPHA];
    int fail;
    int dictSuffix;         /* Nearest ancestor node with a word ending */
    std::vector<int> wordLengths; /* Lengths of words ending at this node */

    TrieNode() : fail(0), dictSuffix(-1) {
        for (int i = 0; i < ALPHA; ++i) children[i] = -1;
    }
};

class AhoCorasickDP {
public:
    AhoCorasickDP() {
        nodes.emplace_back(); /* Root */
    }

    void addPattern(const std::string& p) {
        int cur = 0;
        for (char ch : p) {
            int c = ch - 'a';
            if (nodes[cur].children[c] == -1) {
                nodes[cur].children[c] = static_cast<int>(nodes.size());
                nodes.emplace_back();
            }
            cur = nodes[cur].children[c];
        }
        nodes[cur].wordLengths.push_back(static_cast<int>(p.size()));
    }

    void build() {
        std::queue<int> q;
        for (int c = 0; c < ALPHA; ++c) {
            if (nodes[0].children[c] == -1) {
                nodes[0].children[c] = 0;
            } else {
                nodes[nodes[0].children[c]].fail = 0;
                q.push(nodes[0].children[c]);
            }
        }

        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int c = 0; c < ALPHA; ++c) {
                int v = nodes[u].children[c];
                if (v == -1) {
                    nodes[u].children[c] = nodes[nodes[u].fail].children[c];
                } else {
                    nodes[v].fail = nodes[nodes[u].fail].children[c];
                    /* Dictionary suffix link */
                    if (!nodes[nodes[v].fail].wordLengths.empty()) {
                        nodes[v].dictSuffix = nodes[v].fail;
                    } else {
                        nodes[v].dictSuffix = nodes[nodes[v].fail].dictSuffix;
                    }
                    q.push(v);
                }
            }
        }
    }

    /*
     * Find minimum number of dictionary pieces to form target.
     * Returns -1 if impossible.
     */
    int minPieces(const std::string& target) const {
        int n = static_cast<int>(target.size());
        std::vector<int> dp(n + 1, INT_MAX);
        dp[0] = 0;

        int state = 0;
        for (int i = 0; i < n; ++i) {
            int c = target[i] - 'a';
            state = nodes[state].children[c];

            /* Collect all word lengths from this state and dict suffix chain */
            int temp = state;
            while (temp > 0) {
                for (int len : nodes[temp].wordLengths) {
                    int start = i - len + 1;
                    if (start >= 0 && dp[start] != INT_MAX) {
                        dp[i + 1] = std::min(dp[i + 1], dp[start] + 1);
                    }
                }
                temp = nodes[temp].dictSuffix;
            }
        }

        return dp[n] == INT_MAX ? -1 : dp[n];
    }

    /*
     * Also reconstruct the actual decomposition.
     */
    std::vector<std::string> reconstruct(const std::string& target) const {
        int n = static_cast<int>(target.size());
        std::vector<int> dp(n + 1, INT_MAX);
        std::vector<int> from(n + 1, -1); /* Which length was used */
        dp[0] = 0;

        int state = 0;
        std::vector<int> states(n);
        for (int i = 0; i < n; ++i) {
            int c = target[i] - 'a';
            state = nodes[state].children[c];
            states[i] = state;

            int temp = state;
            while (temp > 0) {
                for (int len : nodes[temp].wordLengths) {
                    int start = i - len + 1;
                    if (start >= 0 && dp[start] != INT_MAX && dp[start] + 1 < dp[i + 1]) {
                        dp[i + 1] = dp[start] + 1;
                        from[i + 1] = len;
                    }
                }
                temp = nodes[temp].dictSuffix;
            }
        }

        std::vector<std::string> result;
        if (dp[n] == INT_MAX) return result;

        int pos = n;
        while (pos > 0) {
            int len = from[pos];
            result.push_back(target.substr(pos - len, len));
            pos -= len;
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    int stateCount() const { return static_cast<int>(nodes.size()); }

private:
    std::vector<TrieNode> nodes;
};

} // namespace aho_dp

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Aho-Corasick + DP: Minimum Dictionary Piece Assembly\n";
    std::cout << "========================================================\n\n";

    /* Test 1 */
    std::vector<std::string> dict1 = {"ab", "abc", "cd", "abcd", "d"};
    std::string target1 = "abcd";

    aho_dp::AhoCorasickDP solver1;
    for (auto& p : dict1) solver1.addPattern(p);
    solver1.build();

    std::cout << "Dictionary: {";
    for (size_t i = 0; i < dict1.size(); ++i) {
        std::cout << "\"" << dict1[i] << "\"";
        if (i + 1 < dict1.size()) std::cout << ", ";
    }
    std::cout << "}\n";
    std::cout << "Target: \"" << target1 << "\"\n";
    std::cout << "Min pieces: " << solver1.minPieces(target1) << "\n";
    auto parts1 = solver1.reconstruct(target1);
    std::cout << "Decomposition: ";
    for (size_t i = 0; i < parts1.size(); ++i) {
        std::cout << "\"" << parts1[i] << "\"";
        if (i + 1 < parts1.size()) std::cout << " + ";
    }
    std::cout << "\n\n";

    /* Test 2 */
    std::vector<std::string> dict2 = {"the", "there", "he", "er", "here"};
    std::string target2 = "thereherethe";

    aho_dp::AhoCorasickDP solver2;
    for (auto& p : dict2) solver2.addPattern(p);
    solver2.build();

    std::cout << "Dictionary: {";
    for (size_t i = 0; i < dict2.size(); ++i) {
        std::cout << "\"" << dict2[i] << "\"";
        if (i + 1 < dict2.size()) std::cout << ", ";
    }
    std::cout << "}\n";
    std::cout << "Target: \"" << target2 << "\"\n";
    std::cout << "Min pieces: " << solver2.minPieces(target2) << "\n";
    auto parts2 = solver2.reconstruct(target2);
    std::cout << "Decomposition: ";
    for (size_t i = 0; i < parts2.size(); ++i) {
        std::cout << "\"" << parts2[i] << "\"";
        if (i + 1 < parts2.size()) std::cout << " + ";
    }
    std::cout << "\n\n";

    /* Test 3: Impossible */
    std::vector<std::string> dict3 = {"aa", "bb"};
    std::string target3 = "aab";

    aho_dp::AhoCorasickDP solver3;
    for (auto& p : dict3) solver3.addPattern(p);
    solver3.build();

    std::cout << "Dictionary: {\"aa\", \"bb\"}\n";
    std::cout << "Target: \"aab\"\n";
    std::cout << "Min pieces: " << solver3.minPieces(target3) << " (expected -1)\n\n";

    /* Test 4: Longer decomposition */
    std::vector<std::string> dict4 = {"a", "ab", "ba", "b"};
    std::string target4 = "ababababab";

    aho_dp::AhoCorasickDP solver4;
    for (auto& p : dict4) solver4.addPattern(p);
    solver4.build();

    std::cout << "Dictionary: {\"a\", \"ab\", \"ba\", \"b\"}\n";
    std::cout << "Target: \"" << target4 << "\"\n";
    std::cout << "Min pieces: " << solver4.minPieces(target4) << "\n";
    auto parts4 = solver4.reconstruct(target4);
    std::cout << "Decomposition: ";
    for (size_t i = 0; i < parts4.size(); ++i) {
        std::cout << "\"" << parts4[i] << "\"";
        if (i + 1 < parts4.size()) std::cout << " + ";
    }
    std::cout << "\n";

    return 0;
}
