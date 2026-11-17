/**
 * ============================================================================
 * Ternary Search Tree (TST / Bentley-Sedgewick String Symbol Table)
 * ============================================================================
 * 
 * Description:
 * A Ternary Search Tree (TST), introduced by Jon Bentley and Robert Sedgewick
 * in 1997, is a trie-like data structure that combines the memory compactness
 * of binary search trees with the fast retrieval time of digital tries.
 * 
 * Node Architecture:
 * Unlike standard tries that allocate an array of 26 or 256 child pointers per
 * node (wasting enormous memory on sparse branches), each TST node contains:
 *   - A character `data`
 *   - Three child pointers:
 *       1. `left`:  for characters strictly less than `data`
 *       2. `equal`: for the NEXT character in the string (matching `data`)
 *       3. `right`: for characters strictly greater than `data`
 *   - A boolean `is_end_of_string` flag.
 * 
 * Supported Operations:
 * 1. Insert: Insert string S in O(|S| + log Sigma) average time.
 * 2. Search: Exact string lookup.
 * 3. Prefix Matching (Autocomplete): Finds all keys starting with a prefix.
 * 4. Wildcard Pattern Matching: Matches patterns with '.' wildcard characters.
 * 
 * Time Complexity:
 * - Search / Insert: O(L + ln N) average, O(L * Sigma) worst case
 * - Space Complexity: O(L * N) nodes (fraction of standard trie memory)
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>

namespace tst {

struct Node {
    char data;
    bool is_end_of_string;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> equal;
    std::unique_ptr<Node> right;

    explicit Node(char c)
        : data(c), is_end_of_string(false), left(nullptr), equal(nullptr), right(nullptr) {}
};

class TernarySearchTree {
public:
    TernarySearchTree() : root(nullptr), word_count(0) {}

    void insert(const std::string& word) {
        if (word.empty()) return;
        root = insertRec(std::move(root), word, 0);
    }

    bool search(const std::string& word) const {
        if (word.empty()) return false;
        Node* curr = searchNode(root.get(), word, 0);
        return (curr != nullptr && curr->is_end_of_string);
    }

    /**
     * Autocomplete: Collects all words stored in the TST starting with prefix.
     */
    std::vector<std::string> keysWithPrefix(const std::string& prefix) const {
        std::vector<std::string> results;
        if (prefix.empty()) {
            collect(root.get(), "", results);
            return results;
        }

        Node* prefix_node = searchNode(root.get(), prefix, 0);
        if (prefix_node == nullptr) return results;

        if (prefix_node->is_end_of_string) {
            results.push_back(prefix);
        }

        collect(prefix_node->equal.get(), prefix, results);
        return results;
    }

    /**
     * Wildcard search: '.' matches any single character.
     */
    std::vector<std::string> wildcardSearch(const std::string& pattern) const {
        std::vector<std::string> results;
        wildcardRec(root.get(), pattern, 0, "", results);
        return results;
    }

    size_t size() const { return word_count; }

private:
    std::unique_ptr<Node> root;
    size_t word_count;

    std::unique_ptr<Node> insertRec(std::unique_ptr<Node> node, const std::string& word, size_t idx) {
        char ch = word[idx];
        if (!node) {
            node = std::make_unique<Node>(ch);
        }

        if (ch < node->data) {
            node->left = insertRec(std::move(node->left), word, idx);
        } else if (ch > node->data) {
            node->right = insertRec(std::move(node->right), word, idx);
        } else {
            if (idx + 1 < word.length()) {
                node->equal = insertRec(std::move(node->equal), word, idx + 1);
            } else {
                if (!node->is_end_of_string) {
                    node->is_end_of_string = true;
                    word_count++;
                }
            }
        }
        return node;
    }

    Node* searchNode(Node* node, const std::string& word, size_t idx) const {
        if (!node) return nullptr;
        char ch = word[idx];

        if (ch < node->data) {
            return searchNode(node->left.get(), word, idx);
        } else if (ch > node->data) {
            return searchNode(node->right.get(), word, idx);
        } else {
            if (idx + 1 == word.length()) {
                return node;
            }
            return searchNode(node->equal.get(), word, idx + 1);
        }
    }

    void collect(Node* node, std::string prefix, std::vector<std::string>& results) const {
        if (!node) return;

        collect(node->left.get(), prefix, results);

        if (node->is_end_of_string) {
            results.push_back(prefix + node->data);
        }
        collect(node->equal.get(), prefix + node->data, results);

        collect(node->right.get(), prefix, results);
    }

    void wildcardRec(Node* node, const std::string& pattern, size_t idx, std::string current, std::vector<std::string>& results) const {
        if (!node) return;
        char ch = pattern[idx];

        if (ch == '.' || ch < node->data) {
            wildcardRec(node->left.get(), pattern, idx, current, results);
        }

        if (ch == '.' || ch == node->data) {
            if (idx + 1 == pattern.length() && node->is_end_of_string) {
                results.push_back(current + node->data);
            }
            if (idx + 1 < pattern.length()) {
                wildcardRec(node->equal.get(), pattern, idx + 1, current + node->data, results);
            }
        }

        if (ch == '.' || ch > node->data) {
            wildcardRec(node->right.get(), pattern, idx, current, results);
        }
    }
};

} // namespace tst

int main() {
    std::cout << "========================================================\n";
    std::cout << " Ternary Search Tree (TST / Bentley-Sedgewick)\n";
    std::cout << "========================================================\n\n";

    tst::TernarySearchTree tree;

    std::vector<std::string> dictionary = {
        "as", "at", "be", "by", "he", "in", "is", "it", "of", "on", "or", "to",
        "bat", "bet", "bit", "bot", "but", "batter", "battle", "ball", "balloon"
    };

    for (const auto& w : dictionary) {
        tree.insert(w);
    }

    std::cout << "Inserted " << tree.size() << " words into the Ternary Search Tree.\n\n";

    std::cout << "1. Exact Word Lookup:\n";
    std::vector<std::string> test_lookups = {"bat", "battle", "hero", "balloon", "xyz", "in"};
    for (const auto& w : test_lookups) {
        bool found = tree.search(w);
        std::cout << "   - \"" << std::setw(8) << std::left << w << "\": "
                  << (found ? "FOUND" : "NOT FOUND") << "\n";
    }

    std::cout << "\n2. Prefix Search (Autocomplete for \"ba\"):\n";
    auto prefix_matches = tree.keysWithPrefix("ba");
    for (const auto& match : prefix_matches) {
        std::cout << "   - " << match << "\n";
    }

    std::cout << "\n3. Wildcard Search (Pattern: \"b.t\"):\n";
    auto wildcard_matches = tree.wildcardSearch("b.t");
    for (const auto& match : wildcard_matches) {
        std::cout << "   - " << match << "\n";
    }

    return 0;
}
