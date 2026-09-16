/*
 * Problem Description:
 * Detect a cycle (loop) in a Linked List. This implementation uses Floyd’s Cycle-Finding Algorithm
 * (also known as the "tortoise and the hare" algorithm) which uses two pointers moving at different speeds.
 */

#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

bool detectLoop(Node* head) {
    Node *slow = head, *fast = head;
    
    while (slow && fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) {
            return true;
        }
    }
    return false;
}

int main() {
    Node* head = new Node(1);
    head->next = new Node(2);
    head->next->next = new Node(3);
    head->next->next->next = new Node(4);
    head->next->next->next->next = new Node(5);
    
    // Create a loop for testing
    head->next->next->next->next->next = head->next;
    
    if (detectLoop(head))
        std::cout << "Loop found in the linked list.\n";
    else
        std::cout << "No loop found.\n";
        
    return 0;
}
