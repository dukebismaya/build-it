/*
 * ============================================================
 *             Binary Buddy System Memory Allocator
 * ============================================================
 *
 * Description:
 *   The Binary Buddy Allocator (Kenneth Knowlton 1965 / Donald
 *   Knuth) manages a power-of-two memory arena [0, 2^MAX_ORDER)
 *   for OS kernel page allocation (Linux kernel zone page
 *   allocator) and real-time engines:
 *
 *     - Maintains free lists `free_blocks[k]` for each order
 *       k = MIN_ORDER .. MAX_ORDER (block size = 2^k bytes).
 *     - Allocate(size):
 *         1. Round size up to the smallest order k with 2^k >= size.
 *         2. Find the smallest order j >= k with a non-empty free
 *            list.
 *         3. Repeatedly split the block of order j down to order k,
 *            pushing the right half ("buddy") onto `free_blocks[j-1]`.
 *     - Free(offset):
 *         1. Compute the block's unique buddy address in O(1) via
 *            XOR: `buddy = offset ^ (1 << k)`.
 *         2. If the buddy is currently free AT THE SAME ORDER k,
 *            remove the buddy, merge them into `min(offset, buddy)`
 *            of order k + 1, and repeat upward until the buddy is
 *            allocated or MAX_ORDER is reached!
 *
 * Complexity:
 *   allocate / deallocate: O(MAX_ORDER - MIN_ORDER) = O(log ArenaSize)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <random>
#include <algorithm>

namespace buddy_alloc {

    class BuddyAllocator {
        int min_order_;
        int max_order_;
        std::vector<std::set<size_t>> free_lists_;       // free_lists_[order] holds block offsets
        std::unordered_map<size_t, int> active_allocs_;  // offset -> order

    public:
        BuddyAllocator(int min_order, int max_order)
            : min_order_(min_order), max_order_(max_order), free_lists_(max_order + 1) {
            free_lists_[max_order_].insert(0);
        }

        size_t total_capacity() const { return size_t(1) << max_order_; }

        // Returns byte offset of allocated block, or (size_t)-1 if out of memory
        size_t allocate(size_t requested_bytes) {
            if (requested_bytes == 0) requested_bytes = 1;
            int target_order = min_order_;
            while (target_order <= max_order_ && (size_t(1) << target_order) < requested_bytes)
                ++target_order;
            if (target_order > max_order_) return static_cast<size_t>(-1);

            int j = target_order;
            while (j <= max_order_ && free_lists_[j].empty()) ++j;
            if (j > max_order_) return static_cast<size_t>(-1);

            size_t offset = *free_lists_[j].begin();
            free_lists_[j].erase(free_lists_[j].begin());

            while (j > target_order) {
                --j;
                size_t buddy = offset + (size_t(1) << j);
                free_lists_[j].insert(buddy);
            }
            active_allocs_[offset] = target_order;
            return offset;
        }

        bool deallocate(size_t offset) {
            auto it = active_allocs_.find(offset);
            if (it == active_allocs_.end()) return false;
            int order = it->second;
            active_allocs_.erase(it);

            while (order < max_order_) {
                size_t buddy = offset ^ (size_t(1) << order);
                auto bit = free_lists_[order].find(buddy);
                if (bit == free_lists_[order].end()) break; // buddy is busy or split
                free_lists_[order].erase(bit);
                offset = std::min(offset, buddy);
                ++order;
            }
            free_lists_[order].insert(offset);
            return true;
        }

        bool is_fully_coalesced() const {
            if (!active_allocs_.empty()) return false;
            for (int k = min_order_; k < max_order_; ++k)
                if (!free_lists_[k].empty()) return false;
            return free_lists_[max_order_].size() == 1 && *free_lists_[max_order_].begin() == 0;
        }

        size_t free_bytes() const {
            size_t sum = 0;
            for (int k = min_order_; k <= max_order_; ++k)
                sum += free_lists_[k].size() * (size_t(1) << k);
            return sum;
        }
    };

} // namespace buddy_alloc

int main() {
    using buddy_alloc::BuddyAllocator;
    std::cout << "============================================================\n";
    std::cout << "             Binary Buddy System Memory Allocator\n";
    std::cout << "============================================================\n\n";

    // 1 MB arena (2^20 bytes), minimum block 64 bytes (2^6)
    BuddyAllocator arena(6, 20);
    std::cout << "Arena capacity = " << arena.total_capacity() << " bytes (2^20, min block = 64 B)\n";

    size_t a = arena.allocate(100);   // rounds to 128 B at offset 0
    size_t b = arena.allocate(200);   // rounds to 256 B at offset 256
    size_t c = arena.allocate(64);    // 64 B at offset 128 (splits the 128 B buddy of 'a'!)
    std::cout << "Allocated 100B -> offset " << a << ", 200B -> offset " << b
              << ", 64B -> offset " << c << "\n";
    arena.deallocate(a);
    arena.deallocate(b);
    arena.deallocate(c);
    std::cout << "After freeing all three, fully coalesced back to 1MB block? "
              << (arena.is_fully_coalesced() ? "YES" : "NO") << "\n";

    std::mt19937 rng(2026);
    struct Block { size_t offset, len; };
    std::vector<Block> live;
    bool ok = arena.is_fully_coalesced();

    for (int step = 0; step < 15000 && ok; ++step) {
        if (live.empty() || (rng() % 10 < 6 && live.size() < 300)) {
            size_t req = 1 + (rng() % 8192);
            size_t off = arena.allocate(req);
            if (off != static_cast<size_t>(-1)) {
                size_t blk_sz = 64;
                while (blk_sz < req) blk_sz <<= 1;
                // Alignment & non-overlap invariant
                if ((off & (blk_sz - 1)) != 0) ok = false;
                for (const auto& ex : live)
                    if (off < ex.offset + ex.len && ex.offset < off + blk_sz) ok = false;
                live.push_back({off, blk_sz});
            }
        } else {
            size_t idx = rng() % live.size();
            if (!arena.deallocate(live[idx].offset)) ok = false;
            live[idx] = live.back();
            live.pop_back();
        }
    }
    for (const auto& ex : live)
        if (!arena.deallocate(ex.offset)) ok = false;
    if (!arena.is_fully_coalesced()) ok = false;

    std::cout << "\nRandom stress test (15,000 alloc/free ops, alignment, disjointness & full coalesce): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
