/**
 * @file palindromic_tree_eertree.cpp
 * @brief Implements the Palindromic Tree (Eertree) for linear-time palindrome processing.
 *
 * Concepts:
 * - Invented by Mikhail Rubinchik and Arseny M. Shur (2015).
 * - Represents all distinct palindromic substrings of a string of length N in O(N) time and space.
 * - Any string of length N has at most N distinct non-empty palindromic substrings.
 * - The tree contains two roots:
 *   - Node 1 (Odd Root): length = -1. Suffix link points to 1.
 *   - Node 2 (Even Root): length = 0 (empty palindrome). Suffix link points to 1.
 * - Transitions: An edge labeled with character c from node P leads to node c P c.
 * - Suffix Links: link[u] points to the node corresponding to the longest proper palindromic suffix of u.
 * - Applications:
 *   - Counting total distinct palindromic substrings.
 *   - Computing frequency of each distinct palindrome in O(N).
 *   - Finding the longest palindromic suffix/substring.
 *
 * Time Complexity: O(N * alphabet_size) or O(N log(alphabet_size)) with map.
 * Space Complexity: O(N)
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>

class PalindromicTree {
public:
    struct Node {
        int len;                     // length of palindromic substring represented
        int link;                    // suffix link (longest proper palindromic suffix)
        std::map<char, int> next;    // transitions: char c -> c P c
        long long count;             // occurrences of this palindrome in the string
        int startPos;                // start position of first occurrence in string
    };

private:
    std::string s;
    std::vector<Node> tree;
    int last;                        // node corresponding to the longest palindromic suffix of current prefix
    int sz;                          // number of nodes

    /**
     * @brief Finds the node whose palindrome can be extended by character s[pos].
     */
    int getLink(int curr, int pos) {
        while (true) {
            int prevPos = pos - 1 - tree[curr].len;
            if (prevPos >= 0 && s[prevPos] == s[pos]) {
                return curr;
            }
            curr = tree[curr].link;
        }
    }

public:
    PalindromicTree() {
        // Reserve index 0 as unused, index 1 = odd root (-1), index 2 = even root (0)
        tree.resize(3);

        tree[1].len = -1;
        tree[1].link = 1;
        tree[1].count = 0;
        tree[1].startPos = -1;

        tree[2].len = 0;
        tree[2].link = 1;
        tree[2].count = 0;
        tree[2].startPos = -1;

        sz = 2;
        last = 2;
    }

    /**
     * @brief Appends a single character to the tree.
     */
    void addChar(char c) {
        s.push_back(c);
        int pos = static_cast<int>(s.length()) - 1;

        int curr = getLink(last, pos);

        if (tree[curr].next.find(c) != tree[curr].next.end()) {
            last = tree[curr].next[c];
            tree[last].count++;
            return;
        }

        // Create new node for palindrome: c + tree[curr] + c
        sz++;
        Node newNode;
        newNode.len = tree[curr].len + 2;
        newNode.count = 1;
        newNode.startPos = pos - newNode.len + 1;

        if (newNode.len == 1) {
            newNode.link = 2; // single character's suffix link is the empty string
        } else {
            newNode.link = tree[getLink(tree[curr].link, pos)].next[c];
        }

        tree.push_back(newNode);
        tree[curr].next[c] = sz;
        last = sz;
    }

    /**
     * @brief Inserts an entire string.
     */
    void build(const std::string& str) {
        for (char c : str) {
            addChar(c);
        }
        propagateCounts();
    }

    /**
     * @brief Propagates occurrence counts from children to their suffix links.
     */
    void propagateCounts() {
        for (int i = sz; i >= 3; --i) {
            tree[tree[i].link].count += tree[i].count;
        }
    }

    int getDistinctPalindromeCount() const {
        return sz - 2; // exclude roots 1 and 2
    }

    struct PalindromeInfo {
        std::string palindrome;
        int length;
        long long frequency;
    };

    std::vector<PalindromeInfo> getAllPalindromes() const {
        std::vector<PalindromeInfo> res;
        for (int i = 3; i <= sz; ++i) {
            std::string sub = s.substr(tree[i].startPos, tree[i].len);
            res.push_back({sub, tree[i].len, tree[i].count});
        }
        return res;
    }
};

int main() {
    std::cout << "=== Palindromic Tree (Eertree) ===\n\n";

    std::vector<std::string> testStrings = {
        "abacaba",
        "bananas",
        "racecar"
    };

    for (const auto& str : testStrings) {
        std::cout << "Processing String: \"" << str << "\" (Length: " << str.length() << ")\n";

        PalindromicTree eertree;
        eertree.build(str);

        std::cout << "  Distinct Palindromes Count: " << eertree.getDistinctPalindromeCount() << '\n';
        auto pals = eertree.getAllPalindromes();

        // Sort by length, then alphabetically
        std::sort(pals.begin(), pals.end(), [](const auto& a, const auto& b) {
            if (a.length != b.length) return a.length < b.length;
            return a.palindrome < b.palindrome;
        });

        std::cout << "  " << std::setw(16) << "Palindrome"
                  << std::setw(10) << "Length"
                  << std::setw(14) << "Occurrences" << '\n';
        std::cout << "  " << std::string(40, '-') << '\n';

        for (const auto& p : pals) {
            std::cout << "  " << std::setw(16) << ("\"" + p.palindrome + "\"")
                      << std::setw(10) << p.length
                      << std::setw(14) << p.frequency << '\n';
        }
        std::cout << '\n';
    }

    return 0;
}
