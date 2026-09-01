#include <algorithm>
#include <iostream>
#include <queue>
#include <climits>

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

// Using queue find the height/depth of the tree

int getLevel(const Node *root, int target, int level) {
  if (root == nullptr)
    return -1;
  if (root->data == target)
    return level;
  int leftLevel = getLevel(root->left, target, level + 1);
  if (leftLevel != -1)
    return leftLevel;
  return getLevel(root->right, target, level + 1);
}
// Using the Level Order traversal Method
int getLevelLOT(Node *root, int target) {
  if (root == nullptr)
    return -1;
  std::queue<Node *> q;
  q.push(root);
  int level{};
  while (!q.empty()) {
    int len = q.size();
    for (int i = 0; i < len; ++i) {
      auto node = q.front();
      q.pop();
      if (node->data == target) {
        return level;
      }
      if (node->left != nullptr)
        q.push(node->left);
      if (node->right != nullptr)
        q.push(node->right);
    }
    level++;
  }
  return -1;
}

auto nodeExists(const Node *root, int key) -> bool {
  if (root == nullptr)
    return false;
  if (root->data == key)
    return true;
  return nodeExists(root->left, key) || nodeExists(root->right, key);
}

auto countNodes(const Node *root) -> int {
  if (root == nullptr)
    return 0;
  return 1 + countNodes(root->left) + countNodes(root->right);
}

auto sumOfNodes(const Node *root) -> int {
  if (root == nullptr)
    return 0;
  return root->data + sumOfNodes(root->left) + sumOfNodes(root->right);
}

auto findMin(const Node *root) -> int {
  if (root == nullptr)
    return INT_MAX;
  int res = root->data;
  int lres = findMin(root->left);
  int rres = findMin(root->right);
  if (lres < res) res = lres;
  if (rres < res) res = rres;
  return res;
}

auto findMax(const Node *root) -> int {
  if (root == nullptr)
    return INT_MIN;
  int res = root->data;
  int lres = findMax(root->left);
  int rres = findMax(root->right);
  if (lres > res) res = lres;
  if (rres > res) res = rres;
  return res;
}

int main() {
  Node *node = new Node(12);
  node->left = new Node(8);
  node->right = new Node(18);
  node->left->left = new Node(5);
  node->left->right = new Node(11);

  std::cout << "Max depth of the tree is: " << height(node) << '\n';

  std::cout << std::boolalpha;
  std::cout << "Does node 5 exist? " << nodeExists(node, 5) << '\n';
  std::cout << "Does node 99 exist? " << nodeExists(node, 99) << '\n';

  std::cout << "Level of node 11 (DFS): " << getLevel(node, 11, 0) << '\n';
  std::cout << "Level of node 18 (BFS): " << getLevelLOT(node, 18) << '\n';

  return 0;
}