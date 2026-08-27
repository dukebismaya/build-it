#include <iostream>
#include <memory>

class Node {
public:
  int data;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  explicit Node(int key) : data{key}, left{nullptr}, right{nullptr} {};
};

int main() {
  /*
       2
      / \
     3  4
    /
   5
  */
  auto root = std::make_unique<Node>(2);
  root->left = std::make_unique<Node>(3);
  root->right = std::make_unique<Node>(4);
  root->left->left = std::make_unique<Node>(5);

  return 0;
}