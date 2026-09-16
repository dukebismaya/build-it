/*
 * Problem Description:
 * Implement an LRU (Least Recently Used) Cache. It should support get and put operations in O(1) time complexity.
 * Uses a doubly linked list and an unordered_map.
 */

#include <iostream>
#include <unordered_map>

class LRUCache {
    struct Node {
        int key, val;
        Node* prev;
        Node* next;
        Node(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    };
    
    int capacity;
    std::unordered_map<int, Node*> map;
    Node* head;
    Node* tail;
    
    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    void addNode(Node* node) {
        node->next = head->next;
        node->next->prev = node;
        head->next = node;
        node->prev = head;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        head = new Node(-1, -1);
        tail = new Node(-1, -1);
        head->next = tail;
        tail->prev = head;
    }
    
    int get(int key) {
        if (map.find(key) != map.end()) {
            Node* resNode = map[key];
            int res = resNode->val;
            map.erase(key);
            removeNode(resNode);
            addNode(resNode);
            map[key] = head->next;
            return res;
        }
        return -1;
    }
    
    void put(int key, int value) {
        if (map.find(key) != map.end()) {
            Node* existingNode = map[key];
            map.erase(key);
            removeNode(existingNode);
        }
        if (map.size() == capacity) {
            map.erase(tail->prev->key);
            removeNode(tail->prev);
        }
        addNode(new Node(key, value));
        map[key] = head->next;
    }
};

int main() {
    LRUCache cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << cache.get(1) << "\n"; // returns 1
    cache.put(3, 3); // evicts key 2
    std::cout << cache.get(2) << "\n"; // returns -1 (not found)
    cache.put(4, 4); // evicts key 1
    std::cout << cache.get(1) << "\n"; // returns -1 (not found)
    std::cout << cache.get(3) << "\n"; // returns 3
    std::cout << cache.get(4) << "\n"; // returns 4
    return 0;
}
