/*
 * Problem Description:
 * Implement a simple Singly Linked List with operations to insert a node at the end
 * and display all nodes in the list.
 */

#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class LinkedList {
private:
    Node* head;
public:
    LinkedList() : head(nullptr) {}
    
    void insert(int data) {
        Node* newNode = new Node(data);
        if(!head) {
            head = newNode;
            return;
        }
        Node* temp = head;
        while(temp->next) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
    
    void display() {
        Node* temp = head;
        while(temp) {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        }
        std::cout << "NULL\n";
    }
};

int main() {
    LinkedList list;
    list.insert(10);
    list.insert(20);
    list.insert(30);
    
    std::cout << "Linked List elements: ";
    list.display();
    
    return 0;
}
