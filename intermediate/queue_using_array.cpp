/*
 * Problem Description:
 * Implement a Queue data structure using an array. It should support basic operations:
 * enqueue (add an element to the rear), dequeue (remove an element from the front), and display.
 */

#include <iostream>

#define MAX 1000

class Queue {
    int front, rear, size;
    int array[MAX];
public:
    Queue() {
        front = size = 0;
        rear = MAX - 1;
    }
    
    bool isFull() { return (size == MAX); }
    bool isEmpty() { return (size == 0); }
    
    void enqueue(int item) {
        if (isFull())
            return;
        rear = (rear + 1) % MAX;
        array[rear] = item;
        size = size + 1;
        std::cout << item << " enqueued to queue\n";
    }
    
    int dequeue() {
        if (isEmpty())
            return INT_MIN;
        int item = array[front];
        front = (front + 1) % MAX;
        size = size - 1;
        return item;
    }
    
    void display() {
        if (isEmpty()) {
            std::cout << "Queue is empty\n";
            return;
        }
        std::cout << "Queue elements: ";
        for(int i = 0, idx = front; i < size; i++, idx = (idx + 1) % MAX) {
            std::cout << array[idx] << " ";
        }
        std::cout << "\n";
    }
};

int main() {
    Queue q;
    q.enqueue(10);
    q.enqueue(20);
    q.enqueue(30);
    q.display();
    std::cout << q.dequeue() << " dequeued from queue\n";
    q.display();
    return 0;
}
