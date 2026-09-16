/*
 * Problem Description:
 * Reverse a singly linked list iteratively. The algorithm changes the next pointer of each 
 * node to point to its previous node.
 */

#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

Node* reverseList(Node* head) {
    Node* prev = nullptr;
    Node* current = head;
    Node* next = nullptr;
    
    while (current != nullptr) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}

void printList(Node* node) {
    while (node != nullptr) {
        std::cout << node->data << " ";
        node = node->next;
    }
    std::cout << "\n";
}

int main() {
    Node* head = new Node(1);
    head->next = new Node(2);
    head->next->next = new Node(3);
    head->next->next->next = new Node(4);
    head->next->next->next->next = new Node(5);
    
    std::cout << "Given linked list: ";
    printList(head);
    
    head = reverseList(head);
    
    std::cout << "Reversed linked list: ";
    printList(head);
    
    return 0;
}
