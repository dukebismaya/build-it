
#include <iostream>
class Node {

public:
  int data;
  Node *prev;
  Node *next;
  explicit Node(int key) : data{key}, prev{nullptr}, next{nullptr} {}
  ~Node() {
    delete prev;
    delete next;
  }
};

Node *insertAtBeginning(Node *head, int data) {
  Node *newNode = new Node(data);
  newNode->next = head;
  if (head != nullptr)
    head->prev = newNode;
  return newNode;
}

Node *insertAtEnd(Node *head, int data) {
  if (head == nullptr)
    return insertAtBeginning(head, data);
  Node *newNode = new Node(data);
  Node *curr = head;
  while (curr->next != nullptr) {
    curr = curr->next;
  }
  curr->next = newNode;
  newNode->prev = curr;
  return head;
}

Node *insertAtAnyPos(Node *head, int data, int pos) {
  if (pos <= 1 || head == nullptr)
    return insertAtBeginning(head, data);

  Node *curr = head;
  for (int i = 1; i < pos - 1 && curr->next != nullptr; ++i) {
    curr = curr->next;
  }

  Node *newNode = new Node(data);
  newNode->next = curr->next;
  newNode->prev = curr;

  if (curr->next != nullptr) {
    curr->next->prev = newNode;
  }
  curr->next = newNode;

  return head;
}

void printList(Node *head) {
  Node *curr{head};
  while (curr != nullptr) {
    std::cout << curr->data;
    if (curr->next != nullptr)
      std::cout << "<->";
    curr = curr->next;
  }
  std::cout << std::endl;
}

int main() {
  Node *head = new Node(2);
  head->next = new Node(3);
  head->next->prev = head;
  head->next->next = new Node(4);
  head->next->next->prev = head->next;
  printList(head);

  return 0;
}