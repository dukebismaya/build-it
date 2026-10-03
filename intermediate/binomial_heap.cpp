/*
 * Problem Description:
 * Implement a Binomial Heap conceptually.
 * A binomial heap is a priority queue implemented as a collection of binomial trees.
 */

#include <iostream>
#include <list>
using namespace std;

struct Node {
    int val, degree;
    Node *parent, *child, *sibling;
    
    Node(int v) {
        val = v;
        degree = 0;
        parent = child = sibling = nullptr;
    }
};

class BinomialHeap {
private:
    list<Node*> _heap;

    list<Node*> unionBHeaps(list<Node*> l1, list<Node*> l2) {
        list<Node*> _new;
        list<Node*>::iterator it = l1.begin();
        list<Node*>::iterator ot = l2.begin();
        while (it != l1.end() && ot != l2.end()) {
            if ((*it)->degree <= (*ot)->degree) {
                _new.push_back(*it);
                it++;
            } else {
                _new.push_back(*ot);
                ot++;
            }
        }
        while (it != l1.end()) {
            _new.push_back(*it);
            it++;
        }
        while (ot != l2.end()) {
            _new.push_back(*ot);
            ot++;
        }
        return _new;
    }

public:
    void insert(int val) {
        Node* temp = new Node(val);
        list<Node*> tempHeap;
        tempHeap.push_back(temp);
        _heap = unionBHeaps(_heap, tempHeap);
        // Note: Full insertion logic requires adjusting the heap to maintain properties
        cout << "Inserted " << val << " into Binomial Heap.\n";
    }

    void display() {
        cout << "Binomial Heap degrees: ";
        for (auto n : _heap) {
            cout << n->degree << " ";
        }
        cout << endl;
    }
};

int main() {
    BinomialHeap bh;
    bh.insert(10);
    bh.insert(20);
    bh.insert(30);

    bh.display();

    return 0;
}
