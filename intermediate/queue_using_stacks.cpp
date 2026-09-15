/*
 * Problem Description:
 * Implement a Queue using two Stacks. The implementation should support standard queue
 * operations like enqueue (pushing to stack 1) and dequeue (transferring to stack 2 to pop).
 */

#include <iostream>
#include <stack>

class Queue {
private:
    std::stack<int> s1, s2;
public:
    void enqueue(int x) {
        s1.push(x);
    }

    int dequeue() {
        if (s1.empty() && s2.empty()) {
            std::cout << "Queue is empty\n";
            return -1;
        }

        if (s2.empty()) {
            while (!s1.empty()) {
                s2.push(s1.top());
                s1.pop();
            }
        }

        int x = s2.top();
        s2.pop();
        return x;
    }
};

int main() {
    Queue q;
    q.enqueue(1);
    q.enqueue(2);
    q.enqueue(3);
    
    std::cout << "Dequeued: " << q.dequeue() << "\n";
    std::cout << "Dequeued: " << q.dequeue() << "\n";
    
    q.enqueue(4);
    std::cout << "Dequeued: " << q.dequeue() << "\n";
    std::cout << "Dequeued: " << q.dequeue() << "\n";
    
    return 0;
}
