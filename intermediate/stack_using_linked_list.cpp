/*
 * Problem Description:
 * Implement a Stack using a Linked List. This avoids the size limitation of an array-based stack.
 * Support operations like push, pop, and display.
 */

#include <iostream>

struct Node {
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class Stack {
private:
    Node* top;
public:
    Stack() : top(nullptr) {}
    
    void push(int x) {
        Node* newNode = new Node(x);
        if (!newNode) {
            std::cout << "Heap Overflow\n";
            return;
        }
        newNode->next = top;
        top = newNode;
        std::cout << x << " pushed to stack\n";
    }
    
    int pop() {
        if (top == nullptr) {
            std::cout << "Stack Underflow\n";
            return -1;
        }
        Node* temp = top;
        int popped = temp->data;
        top = top->next;
        delete temp;
        return popped;
    }
    
    void display() {
        if (top == nullptr) {
            std::cout << "Stack is empty\n";
            return;
        }
        Node* temp = top;
        std::cout << "Stack elements: ";
        while (temp != nullptr) {
            std::cout << temp->data << " -> ";
            temp = temp->next;
        }
        std::cout << "NULL\n";
    }
};

int main() {
    Stack s;
    s.push(11);
    s.push(22);
    s.push(33);
    s.display();
    std::cout << s.pop() << " popped from stack\n";
    s.display();
    return 0;
}
