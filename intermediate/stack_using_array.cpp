/*
 * Problem Description:
 * Implement a Stack data structure using an array. It should support basic operations:
 * push (add an element), pop (remove top element), and display the stack.
 */

#include <iostream>

#define MAX 1000

class Stack {
    int top;
public:
    int a[MAX];
    Stack() { top = -1; }
    
    bool push(int x) {
        if (top >= (MAX - 1)) {
            std::cout << "Stack Overflow\n";
            return false;
        } else {
            a[++top] = x;
            std::cout << x << " pushed into stack\n";
            return true;
        }
    }
    
    int pop() {
        if (top < 0) {
            std::cout << "Stack Underflow\n";
            return 0;
        } else {
            int x = a[top--];
            return x;
        }
    }
    
    void display() {
        if (top < 0) {
            std::cout << "Stack is empty\n";
            return;
        }
        std::cout << "Stack elements: ";
        for (int i = top; i >= 0; i--) {
            std::cout << a[i] << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    Stack s;
    s.push(10);
    s.push(20);
    s.push(30);
    s.display();
    std::cout << s.pop() << " popped from stack\n";
    s.display();
    return 0;
}
