/*
 * Problem Description:
 * Implement a simple Circular Linked List where the last node points back to the first node.
 * Provide operations to insert a node at the end and display all nodes.
 */

#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class CircularLinkedList {
private:
    Node* head;
public:
    CircularLinkedList() : head(nullptr) {}
    
    void insert(int data) {
        Node* newNode = new Node(data);
        if(!head) {
            head = newNode;
            newNode->next = head;
            return;
        }
        Node* temp = head;
        while(temp->next != head) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->next = head;
    }
    
    void display() {
        if(!head) return;
        Node* temp = head;
        std::cout << "Circular Linked List: ";
        do {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        } while(temp != head);
        std::cout << "(HEAD: " << head->data << ")\n";
    }
};

int main() {
    CircularLinkedList list;
    list.insert(10);
    list.insert(20);
    list.insert(30);
    
    list.display();
    
    return 0;
}
