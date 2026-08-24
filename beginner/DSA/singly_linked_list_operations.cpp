#include <cstddef>
#include <iostream>

class Node {
public:
  int data;
  Node *next;
  explicit Node(int data) : data{data}, next{nullptr} {}
  ~Node() { delete next; }
};

Node *insertAtBeginning(Node *head, int data) {
  Node *newNode = new Node(data);
  newNode->next = head;
  return newNode;
}

Node *insertAtEnd(Node *head, int data) {
  Node *newNode = new Node(data);
  if (head == nullptr)
    return newNode;
  Node *lastNode = head;
  while (lastNode->next != nullptr) {
    lastNode = lastNode->next;
  }
  lastNode->next = newNode;
  return head;
}

Node *insertAtAnyPos(Node *head, int data, int pos) {
  if (pos < 1)
    return head;
  if (pos == 1) {
    Node *newNode = new Node(data);
    newNode->next = head;
    return newNode;
  }
  Node *curr = head;
  for (int i = 0; i < pos - 1 && curr != nullptr; ++i) {
    curr = curr->next;
  }
  if (curr == nullptr)
    return head;
  Node *newNode = new Node(data);
  newNode->next = curr->next;
  curr->next = newNode;
  return head;
}

void traverseLinkedList(Node *head) {
  while (head != nullptr) {
    std::cout << head->data;
    if (head->next != nullptr)
      std::cout << "->";
    head = head->next;
  }
  std::cout << std::endl;
}

int main() {
  Node *head = new Node(10);
  head->next = new Node(20);
  head->next->next = new Node(30);
  head->next->next->next = new Node(40);
  // Print the list
  //   traverseLinkedList(head);
  // Insert at the beginning
  int toInsert = 0;
  head = insertAtBeginning(head, toInsert);
  std::cout << "Inserting " << toInsert << " at beginning: ";
  traverseLinkedList(head);
  toInsert = 60;
  std::cout << "Inserting " << toInsert << " at end: ";
  head = insertAtEnd(head, toInsert);
  traverseLinkedList(head);
  toInsert = 50;
  int pos = 5;
  std::cout << "Inserting " << toInsert << " at " << pos << ": ";
  head = insertAtAnyPos(head, toInsert, pos);
  traverseLinkedList(head);

  return 0;
}