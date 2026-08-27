#include <iostream>
#include <memory>
#include <vector>

class Node {
public:
  int data;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  explicit Node(int data) : data{data}, left{nullptr}, right{nullptr} {}
};

void inOrderTraversal(const Node *node, std::vector<int> &res) {
  if (node == nullptr)
    return;
  inOrderTraversal(node->left.get(), res);
  res.push_back(node->data);
  inOrderTraversal(node->right.get(), res);
}

void preOrderTraversal(const Node *node, std::vector<int> &res) {
  if (node == nullptr)
    return;
  res.push_back(node->data);
  preOrderTraversal(node->left.get(), res);
  preOrderTraversal(node->right.get(), res);
}

int main() {
  auto root = std::make_unique<Node>(1);
  root->left = std::make_unique<Node>(2);
  root->right = std::make_unique<Node>(3);
  root->left->left = std::make_unique<Node>(4);
  root->left->right = std::make_unique<Node>(5);
  root->right->right = std::make_unique<Node>(6);
  std::vector<int> res;
  inOrderTraversal(root.get(), res);
  std::cout << "In-order Traversal: ";
  std::cout << std::format("{}", res) << std::endl;
  return 0;
}