/*
 * Problem Description:
 * Implement a Fibonacci Heap conceptually.
 * A Fibonacci heap is a data structure for priority queue operations, consisting of a 
 * collection of heap-ordered trees.
 */

#include <iostream>
#include <cmath>
using namespace std;

struct Node {
    int key;
    int degree;
    Node* parent;
    Node* child;
    Node* left;
    Node* right;
    bool mark;
    
    Node(int val) {
        key = val;
        degree = 0;
        parent = nullptr;
        child = nullptr;
        left = this;
        right = this;
        mark = false;
    }
};

class FibonacciHeap {
private:
    Node* min_node;
    int num_nodes;

public:
    FibonacciHeap() {
        min_node = nullptr;
        num_nodes = 0;
    }

    void insert(int val) {
        Node* new_node = new Node(val);
        if (min_node == nullptr) {
            min_node = new_node;
        } else {
            // Insert new_node into root list
            (min_node->left)->right = new_node;
            new_node->right = min_node;
            new_node->left = min_node->left;
            min_node->left = new_node;
            
            if (val < min_node->key) {
                min_node = new_node;
            }
        }
        num_nodes++;
        cout << "Inserted " << val << " into Fibonacci Heap.\n";
    }

    int getMin() {
        if (min_node != nullptr)
            return min_node->key;
        return -1;
    }
};

int main() {
    FibonacciHeap fh;
    fh.insert(10);
    fh.insert(2);
    fh.insert(15);
    fh.insert(6);

    cout << "Minimum value in Fibonacci Heap: " << fh.getMin() << endl;

    return 0;
}
