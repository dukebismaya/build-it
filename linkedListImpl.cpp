struct Node {

  int data;
  Node *next;
  Node(int data) : data{data}, next{nullptr} {}
};

class Stack {
  Node *head;
  int top;
  int MAX_CAPACITY{10};

public:
  Stack() { top = -1; }
  bool isEmpty() { return top == -1; }
  bool isFull() { return top == MAX_CAPACITY; }
  void push(int el) {
    if (isFull())
      return;
    Node *newNode = new Node(el);
    head->next = newNode;
    top++;
  }
  int pop() {
    if (isEmpty())
      return -1;
    Node *temp = head;
    int value = temp->data;
    head = head->next;
    delete temp;
    return value;
  }
};