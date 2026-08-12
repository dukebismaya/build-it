#include <vector>
struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
  ListNode *mergeKLists(std::vector<ListNode *> &lists) {
    ListNode *res = new ListNode(0);
    ListNode *cur = res;
    while (true) {
      int minNode = -1;
      for (int i = 0; i < lists.size(); ++i) {
        if (!lists[i])
          continue;
        if (minNode == -1 || lists[minNode]->val > lists[i]->val)
          minNode = i;
      }
      if (minNode == -1)
        break;
      cur->next = lists[minNode];
      lists[minNode] = lists[minNode]->next;
      cur = cur->next;
    }
    return res->next;
  }
};
