/*
 * Problem Description:
 * Implement a Radix Tree (Patricia Trie) conceptually.
 * A Radix tree is a space-optimized trie in which each node that is the only child 
 * is merged with its parent.
 */

#include <iostream>
#include <map>
#include <string>
using namespace std;

class RadixTreeNode {
public:
    bool isEndOfWord;
    map<string, RadixTreeNode*> children;

    RadixTreeNode() {
        isEndOfWord = false;
    }
};

class RadixTree {
private:
    RadixTreeNode* root;

public:
    RadixTree() {
        root = new RadixTreeNode();
    }

    // A complete radix tree insertion involves splitting edges if a partial match occurs.
    // This is a simplified conceptual structure.
    void insert(string word) {
        // Simplified placeholder: acts somewhat like a standard trie here for demonstration
        root->children[word] = new RadixTreeNode();
        root->children[word]->isEndOfWord = true;
    }

    bool search(string word) {
        if (root->children.find(word) != root->children.end()) {
            return root->children[word]->isEndOfWord;
        }
        return false;
    }
};

int main() {
    RadixTree rt;
    rt.insert("hello");
    rt.insert("world");

    cout << "Search 'hello': " << rt.search("hello") << endl;
    cout << "Search 'word': " << rt.search("word") << endl;

    return 0;
}
