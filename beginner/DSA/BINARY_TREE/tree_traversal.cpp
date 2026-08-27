#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include <format>

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

void postOrderTraversal(const Node *node, std::vector<int> &res) {
  if (node == nullptr)
    return;
  postOrderTraversal(node->left.get(), res);
  postOrderTraversal(node->right.get(), res);
  res.push_back(node->data);
}

// Level order traversal using Recursion
void levelOrderTraversalRecursion(const Node *root,
                                  std::vector<std::vector<int>> &res,
                                  int level) {
  if (root == nullptr)
    return;
  if (res.size() <= level) {
    res.push_back({});
  }
  res[level].push_back(root->data);
  levelOrderTraversalRecursion(root->left.get(), res, level + 1);
  levelOrderTraversalRecursion(root->right.get(), res, level + 1);
}

auto levelOrderTraversal(const Node *root) -> std::vector<std::vector<int>> {
  std::vector<std::vector<int>> res;
  levelOrderTraversalRecursion(root, res, 0);
  return res;
}

// Level order traversal using queue
auto levelOrderTraversalQueue(Node *root) -> std::vector<std::vector<int>> {
  if (root == nullptr)
    return {};
  std::queue<Node *> q;
  q.push(root);
  std::vector<std::vector<int>> res;
  int curr_level{};
  while (!q.empty()) {
    int len = q.size();
    res.push_back({});
    for (int i = 0; i < len; ++i) {
      auto node = q.front();
      q.pop();
      res[curr_level].push_back(node->data);
      if (node->left != nullptr)
        q.push(node->left.get());
      if (node->right != nullptr)
        q.push(node->right.get());
    }
    curr_level++;
  }
  return res;
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