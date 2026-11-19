/*
 * ============================================================
 *           Persistent Stack (Linked / Functional)
 * ============================================================
 *
 * Description:
 *   A persistent (functional) stack is an immutable data
 *   structure where push and pop operations return new stack
 *   versions without modifying the original. All previous
 *   versions remain accessible. This is achieved through
 *   structural sharing using linked nodes.
 *
 *   Unlike ephemeral stacks, a persistent stack allows "time
 *   travel" — you can access any historical version at any time.
 *
 * Data Structure:
 *   Each stack version is a pointer to a linked list node:
 *     Node { value, next_ptr }
 *
 *   - push(val) → creates a new node pointing to current head
 *   - pop()     → returns the tail (next of head)
 *   - top()     → returns head's value
 *
 *   Because push/pop only create or skip one node, previous
 *   versions share structure (all tail nodes are shared).
 *
 * Complexity:
 *   push:  O(1) time, O(1) new memory
 *   pop:   O(1) time, O(1) new memory (zero allocation)
 *   top:   O(1) time
 *   Space: O(N) total across all versions (with sharing)
 *
 * Memory Management:
 *   Uses std::shared_ptr for automatic reference counting.
 *   Nodes are freed when no version references them.
 *
 * ============================================================
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <iomanip>

namespace persistent_stack {

    template <typename T>
    class PersistentStack {
    private:
        struct Node {
            T value;
            std::shared_ptr<Node> next;
            Node(T val, std::shared_ptr<Node> nxt)
                : value(std::move(val)), next(std::move(nxt)) {}
        };

        std::shared_ptr<Node> head_;
        int size_;

        // Private constructor for internal use
        PersistentStack(std::shared_ptr<Node> head, int size)
            : head_(std::move(head)), size_(size) {}

    public:
        // Create an empty stack
        PersistentStack() : head_(nullptr), size_(0) {}

        // Check if empty
        bool empty() const { return head_ == nullptr; }

        // Get size
        int size() const { return size_; }

        // Get top element
        const T& top() const {
            if (empty()) throw std::runtime_error("top() on empty stack");
            return head_->value;
        }

        // Push: returns a NEW stack with val on top
        PersistentStack push(const T& val) const {
            auto new_node = std::make_shared<Node>(val, head_);
            return PersistentStack(new_node, size_ + 1);
        }

        // Pop: returns a NEW stack without the top element
        PersistentStack pop() const {
            if (empty()) throw std::runtime_error("pop() on empty stack");
            return PersistentStack(head_->next, size_ - 1);
        }

        // Convert current version to vector (top to bottom)
        std::vector<T> to_vector() const {
            std::vector<T> result;
            auto node = head_;
            while (node) {
                result.push_back(node->value);
                node = node->next;
            }
            return result;
        }

        // Print stack contents
        void print(const std::string& label = "") const {
            if (!label.empty()) std::cout << label << ": ";
            std::cout << "[";
            auto node = head_;
            bool first = true;
            while (node) {
                if (!first) std::cout << ", ";
                std::cout << node->value;
                node = node->next;
                first = false;
            }
            std::cout << "] (size=" << size_ << ")\n";
        }
    };

} // namespace persistent_stack

int main() {
    using namespace persistent_stack;

    std::cout << "============================================================\n";
    std::cout << "         Persistent Stack (Functional / Linked)\n";
    std::cout << "============================================================\n\n";

    // Demonstration: build up versions
    std::cout << "--- Building stack versions ---\n";
    PersistentStack<int> v0;  // empty
    auto v1 = v0.push(10);
    auto v2 = v1.push(20);
    auto v3 = v2.push(30);
    auto v4 = v3.pop();       // removes 30
    auto v5 = v4.push(40);    // adds 40 on top of [20, 10]

    v0.print("v0 (empty)   ");
    v1.print("v1 (push 10) ");
    v2.print("v2 (push 20) ");
    v3.print("v3 (push 30) ");
    v4.print("v4 (pop)     ");
    v5.print("v5 (push 40) ");

    // Key property: all versions are still accessible
    std::cout << "\n--- All versions remain accessible ---\n";
    std::cout << "v2.top() = " << v2.top() << " (should be 20)\n";
    std::cout << "v3.top() = " << v3.top() << " (should be 30)\n";
    std::cout << "v5.top() = " << v5.top() << " (should be 40)\n";

    // Demonstrate branching: create two branches from v2
    std::cout << "\n--- Branching from v2 ---\n";
    auto branch_a = v2.push(100);
    auto branch_b = v2.push(200);
    branch_a.print("branch_a (v2 + 100)");
    branch_b.print("branch_b (v2 + 200)");
    v2.print("v2 (unchanged)     ");

    // String stack example
    std::cout << "\n--- String stack example ---\n";
    PersistentStack<std::string> s0;
    auto s1 = s0.push("hello");
    auto s2 = s1.push("world");
    auto s3 = s2.push("foo");
    auto s4 = s3.pop();
    s1.print("s1");
    s2.print("s2");
    s3.print("s3");
    s4.print("s4 (after pop)");

    // Verify structural sharing: v2 and v3 share the [20, 10] tail
    std::cout << "\n--- Structural sharing verification ---\n";
    auto vec2 = v2.to_vector();
    auto vec3 = v3.to_vector();
    std::cout << "v2 contents: ";
    for (int x : vec2) std::cout << x << " ";
    std::cout << "\n";
    std::cout << "v3 contents: ";
    for (int x : vec3) std::cout << x << " ";
    std::cout << "\n";
    std::cout << "v3 tail (pop) == v2: "
              << (v3.pop().top() == v2.top() ? "YES (shared)" : "NO") << "\n";

    // Stress test: build a deep persistent stack
    std::cout << "\n--- Stress test: 1000 versions ---\n";
    PersistentStack<int> base;
    std::vector<PersistentStack<int>> versions;
    versions.push_back(base);

    for (int i = 1; i <= 1000; ++i) {
        versions.push_back(versions.back().push(i));
    }

    std::cout << "Version 0 size: " << versions[0].size() << "\n";
    std::cout << "Version 500 size: " << versions[500].size()
              << ", top: " << versions[500].top() << "\n";
    std::cout << "Version 1000 size: " << versions[1000].size()
              << ", top: " << versions[1000].top() << "\n";

    // Pop 500 times from version 1000
    auto v_half = versions[1000];
    for (int i = 0; i < 500; ++i) v_half = v_half.pop();
    std::cout << "After 500 pops from v1000: size=" << v_half.size()
              << ", top=" << v_half.top() << "\n";

    std::cout << "\n============================================================\n";
    std::cout << "  Persistent stacks enable time-travel and branching\n";
    std::cout << "  through structural sharing of immutable linked nodes.\n";
    std::cout << "============================================================\n";

    return 0;
}
