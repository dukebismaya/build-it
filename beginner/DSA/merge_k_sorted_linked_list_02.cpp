#include <vector>
struct ListNode {
  int val;
  ListNode *next;
  ListNode() : val(0), next(nullptr) {}
  ListNode(int x) : val(x), next(nullptr) {}
  ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
private:
  ListNode *merge(ListNode *l1, ListNode *l2) {
    ListNode dummy(0);
    ListNode *curr = &dummy;

    while (l1 != nullptr && l2 != nullptr) {
      if (l1->val <= l2->val) {
        curr->next = l1;
        l1 = l1->next;
      } else {
        curr->next = l2;
        l2 = l2->next;
      }
      curr = curr->next;
    }

    if (l1 != nullptr)
      curr->next = l1;
    else
      curr->next = l2;

    return dummy.next;
  }

public:
  ListNode *mergeKLists(std::vector<ListNode *> &lists) {
    if (lists.empty())
      return nullptr;

    int interval = 1;
    while (interval < lists.size()) {
      for (int i = 0; i + interval < lists.size(); i = i + interval * 2) {
        lists[i] = merge(lists[i], lists[i + interval]);
      }
      interval *= 2;
    }

    return lists[0];
  }
};