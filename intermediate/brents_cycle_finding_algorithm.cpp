/*
 * Problem Description:
 * Implement Brent's Cycle Finding Algorithm.
 * Brent's algorithm is a cycle detection algorithm, conceptually similar to Floyd's tortoise and hare, 
 * but uses a moving tortoise and a tele-porting hare, reducing the number of function evaluations.
 */

#include <iostream>
using namespace std;

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class BrentCycleFinding {
public:
    Node* brent(Node* head) {
        if (!head || !head->next) return nullptr;
        
        Node* tortoise = head;
        Node* hare = head->next;
        int power = 1;
        int length = 1;
        
        // Find length of the cycle
        while (tortoise != hare) {
            if (length == power) {
                tortoise = hare;
                power *= 2;
                length = 0;
            }
            if (!hare || !hare->next) return nullptr; // No cycle
            hare = hare->next;
            length++;
        }
        
        // Find the start of the cycle
        tortoise = head;
        hare = head;
        for (int i = 0; i < length; i++) {
            hare = hare->next;
        }
        
        while (tortoise != hare) {
            tortoise = tortoise->next;
            hare = hare->next;
        }
        
        return tortoise; // Start node of cycle
    }
};

int main() {
    Node* head = new Node(1);
    head->next = new Node(2);
    head->next->next = new Node(3);
    head->next->next->next = new Node(4);
    head->next->next->next->next = new Node(5);
    
    // Create a cycle for testing: 5 points back to 3
    head->next->next->next->next->next = head->next->next; 

    BrentCycleFinding bcf;
    Node* cycleStart = bcf.brent(head);
    
    if (cycleStart) {
        cout << "Cycle detected! Starts at node with data: " << cycleStart->data << "\n";
    } else {
        cout << "No cycle detected.\n";
    }

    return 0;
}
