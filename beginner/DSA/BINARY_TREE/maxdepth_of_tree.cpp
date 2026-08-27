#include <algorithm>
#include <iostream>

class Node {
public:
  int data;
  Node *left, *right;
  explicit Node(int key) : data{key}, left{nullptr}, right{nullptr} {}
  ~Node() {
    delete left;
    delete right;
  }
};

auto height(const Node *root) -> int {
  if (root == nullptr)
    return -1;
  int lheight = height(root->left);
  int rheight = height(root->right);
  return std::max(lheight, rheight) + 1;
}

int main() {
  Node *node = new Node(12);
  node->left = new Node(8);
  node->right = new Node(18);
  node->left->left = new Node(5);
  node->left->right = new Node(11);

  std::cout << "Max depth of the tree is: " << height(node);

  return 0;
}