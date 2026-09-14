/*
 * Problem Description:
 * Implement a simple Doubly Linked List with operations to insert a node at the end
 * and display all nodes in both forward and backward directions.
 */

#include <iostream>

struct Node {
    int data;
    Node* prev;
    Node* next;
    Node(int val) : data(val), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
private:
    Node* head;
    Node* tail;
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}
    
    void insert(int data) {
        Node* newNode = new Node(data);
        if(!head) {
            head = tail = newNode;
            return;
        }
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    
    void displayForward() {
        Node* temp = head;
        std::cout << "Forward: ";
        while(temp) {
            std::cout << temp->data << " <-> ";
            temp = temp->next;
        }
        std::cout << "NULL\n";
    }
    
    void displayBackward() {
        Node* temp = tail;
        std::cout << "Backward: ";
        while(temp) {
            std::cout << temp->data << " <-> ";
            temp = temp->prev;
        }
        std::cout << "NULL\n";
    }
};

int main() {
    DoublyLinkedList list;
    list.insert(10);
    list.insert(20);
    list.insert(30);
    
    list.displayForward();
    list.displayBackward();
    
    return 0;
}
