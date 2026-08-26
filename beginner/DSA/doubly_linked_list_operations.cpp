
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