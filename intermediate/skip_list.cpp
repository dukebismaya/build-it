/*
 * Problem Description:
 * Implement a Skip List conceptually.
 * A skip list is a probabilistic data structure that allows O(log n) search complexity 
 * as well as O(log n) insertion complexity within an ordered sequence of n elements.
 */

#include <iostream>
#include <vector>
using namespace std;

class Node {
public:
    int key;
    vector<Node*> forward;

    Node(int k, int level) {
        key = k;
        forward.resize(level + 1, nullptr);
    }
};

class SkipList {
private:
    int MAXLVL;
    float P;
    int level;
    Node* header;

    int randomLevel() {
        int lvl = 0;
        // Conceptual probability simulation
        while ((rand() % 100) < (P * 100) && lvl < MAXLVL)
            lvl++;
        return lvl;
    }

public:
    SkipList(int max_lvl, float p) {
        MAXLVL = max_lvl;
        P = p;
        level = 0;
        header = new Node(-1, MAXLVL);
    }

    void insertElement(int key) {
        Node* current = header;
        vector<Node*> update(MAXLVL + 1, nullptr);

        for (int i = level; i >= 0; i--) {
            while (current->forward[i] != nullptr && current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current == nullptr || current->key != key) {
            int rlevel = randomLevel();

            if (rlevel > level) {
                for (int i = level + 1; i <= rlevel; i++) {
                    update[i] = header;
                }
                level = rlevel;
            }

            Node* n = new Node(key, rlevel);

            for (int i = 0; i <= rlevel; i++) {
                n->forward[i] = update[i]->forward[i];
                update[i]->forward[i] = n;
            }
            cout << "Successfully Inserted key " << key << "\n";
        }
    }
};

int main() {
    SkipList lst(3, 0.5);
    
    lst.insertElement(3);
    lst.insertElement(6);
    lst.insertElement(7);
    lst.insertElement(9);
    lst.insertElement(12);

    return 0;
}
