/*
 * Problem Description:
 * Floyd's Cycle-Finding Algorithm (Tortoise and Hare Algorithm).
 * Given a singly linked list, determine whether it contains a cycle without using extra memory.
 * If a cycle exists, find:
 * 1. The starting node (entry point) of the cycle.
 * 2. The length (number of nodes) of the cycle.
 * The algorithm uses two pointers:
 * - Slow pointer (Tortoise): advances 1 step at a time.
 * - Fast pointer (Hare): advances 2 steps at a time.
 * Time Complexity: O(N) where N is the number of nodes.
 * Space Complexity: O(1) auxiliary memory.
 */

#include <iostream>

struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// Detects cycle and returns entry node if present, otherwise nullptr
ListNode* detectCycleEntry(ListNode* head) {
    if (!head || !head->next) return nullptr;

    ListNode* slow = head;
    ListNode* fast = head;

    // Phase 1: Determine if cycle exists
    bool hasCycle = false;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            hasCycle = true;
            break;
        }
    }

    if (!hasCycle) return nullptr;

    // Phase 2: Locate cycle start node
    ListNode* ptr1 = head;
    ListNode* ptr2 = slow;
    while (ptr1 != ptr2) {
        ptr1 = ptr1->next;
        ptr2 = ptr2->next;
    }

    return ptr1;
}

// Calculates length of cycle
int getCycleLength(ListNode* cycleNode) {
    if (!cycleNode) return 0;
    ListNode* curr = cycleNode->next;
    int length = 1;
    while (curr != cycleNode) {
        length++;
        curr = curr->next;
    }
    return length;
}

int main() {
    // Construct linked list: 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> (back to 3)
    ListNode* node1 = new ListNode(1);
    ListNode* node2 = new ListNode(2);
    ListNode* node3 = new ListNode(3);
    ListNode* node4 = new ListNode(4);
    ListNode* node5 = new ListNode(5);
    ListNode* node6 = new ListNode(6);

    node1->next = node2;
    node2->next = node3;
    node3->next = node4;
    node4->next = node5;
    node5->next = node6;
    node6->next = node3; // Creates cycle of length 4 starting at node3 (val: 3)

    ListNode* entry = detectCycleEntry(node1);
    if (entry) {
        int len = getCycleLength(entry);
        std::cout << "Cycle detected in linked list!\n";
        std::cout << "Cycle begins at node with value: " << entry->val << "\n";
        std::cout << "Length of cycle: " << len << " nodes\n";
    } else {
        std::cout << "No cycle detected in linked list.\n";
    }

    // Clean up nodes (break loop before deleting)
    node6->next = nullptr;
    ListNode* curr = node1;
    while (curr) {
        ListNode* next = curr->next;
        delete curr;
        curr = next;
    }

    return 0;
}
