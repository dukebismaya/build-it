/*
 * Problem Description:
 * Find the middle element of a Linked List. We use a slow and fast pointer approach. 
 * The slow pointer moves one step at a time while the fast pointer moves two steps.
 * When the fast pointer reaches the end, the slow pointer will be at the middle.
 */

#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

void printMiddle(Node* head) {
    Node* slow = head;
    Node* fast = head;
    
    if (head != nullptr) {
        while (fast != nullptr && fast->next != nullptr) {
            fast = fast->next->next;
            slow = slow->next;
        }
        std::cout << "The middle element is [" << slow->data << "]\n";
    }
}

int main() {
    Node* head = new Node(10);
    head->next = new Node(20);
    head->next->next = new Node(30);
    head->next->next->next = new Node(40);
    head->next->next->next->next = new Node(50);
    
    printMiddle(head);
    
    return 0;
}
