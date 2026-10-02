/*
 * Problem Description:
 * Implement a Suffix Tree conceptually.
 * A Suffix Tree is a compressed trie containing all the suffixes of the given text as their keys and positions in the text as their values.
 */

#include <iostream>
#include <map>
#include <string>
using namespace std;

// This is a naive implementation of a Suffix Trie (not a compressed Suffix Tree) for demonstration.
// Ukkonen's algorithm is typically used for O(n) Suffix Tree construction.

class SuffixTrieNode {
public:
    map<char, SuffixTrieNode*> children;
    
    void insertSuffix(string suffix) {
        if (suffix.empty()) return;
        char firstChar = suffix[0];
        if (children.find(firstChar) == children.end()) {
            children[firstChar] = new SuffixTrieNode();
        }
        children[firstChar]->insertSuffix(suffix.substr(1));
    }
};

class SuffixTrie {
    SuffixTrieNode* root;
public:
    SuffixTrie(string text) {
        root = new SuffixTrieNode();
        for (int i = 0; i < text.length(); i++) {
            root->insertSuffix(text.substr(i));
        }
    }
    
    bool search(string pattern) {
        SuffixTrieNode* current = root;
        for (char ch : pattern) {
            if (current->children.find(ch) == current->children.end()) {
                return false;
            }
            current = current->children[ch];
        }
        return true;
    }
};

int main() {
    string text = "banana";
    SuffixTrie st(text);
    
    cout << "Search 'nan': " << (st.search("nan") ? "Found" : "Not Found") << endl;
    cout << "Search 'apple': " << (st.search("apple") ? "Found" : "Not Found") << endl;
    
    return 0;
}
