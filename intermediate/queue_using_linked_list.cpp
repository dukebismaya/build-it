/*
 * Problem Description:
 * Implement a Queue using a Linked List. The queue should support enqueue (add to rear)
 * and dequeue (remove from front) operations efficiently.
 */

#include <iostream>

struct QNode {
    int data;
    QNode* next;
    QNode(int d) : data(d), next(nullptr) {}
};

class Queue {
    QNode *front, *rear;
public:
    Queue() {
        front = rear = nullptr;
    }

    void enqueue(int x) {
        QNode* temp = new QNode(x);

        if (rear == nullptr) {
            front = rear = temp;
            return;
        }

        rear->next = temp;
        rear = temp;
    }

    void dequeue() {
        if (front == nullptr)
            return;

        QNode* temp = front;
        front = front->next;

        if (front == nullptr)
            rear = nullptr;

        delete temp;
    }
    
    void display() {
        if(!front) {
            std::cout << "Queue is empty\n";
            return;
        }
        QNode* temp = front;
        std::cout << "Queue elements: ";
        while (temp != nullptr) {
            std::cout << temp->data << " ";
            temp = temp->next;
        }
        std::cout << "\n";
    }
};

int main() {
    Queue q;
    q.enqueue(10);
    q.enqueue(20);
    q.dequeue();
    q.dequeue();
    q.enqueue(30);
    q.enqueue(40);
    q.enqueue(50);
    q.dequeue();
    
    q.display();
    
    return 0;
}
