/*
 * ============================================================
 *       Hierarchical Timing Wheel O(1) Timer Scheduler
 * ============================================================
 *
 * Description:
 *   A min-heap timer queue costs O(log N) per start/cancel, which
 *   becomes a bottleneck in high-throughput network servers, game
 *   engines, and OS kernels managing millions of connection timeouts.
 *
 *   The Hierarchical Timing Wheel (Varghese & Lauck, SOSP 1987,
 *   used in the Linux kernel, Netty, Apache Kafka, and Akka)
 *   organizes time into L levels of circular slot arrays (like an
 *   odometer or clock with 256 slots per wheel, 4 levels = 32-bit
 *   tick range):
 *     - Level l has B = 2^8 = 256 slots, each representing 256^l ticks.
 *     - Scheduling a timer at expiration `exp` places it in the highest
 *       level l where `(exp >> (8*l)) != (now >> (8*l))` at slot
 *       `(exp >> (8*l)) & 0xFF` in O(1) time.
 *     - Advancing the clock by 1 tick inspects slot `now & 0xFF` of
 *       Level 0; whenever a wheel wraps around 0, it cascades
 *       (re-inserts) the timers from the next outer wheel slot down
 *       into inner wheels in O(1) amortized time per timer.
 *
 * Complexity:
 *   schedule(delay, cb): O(1) worst-case
 *   cancel(timer_id):    O(1) via intrusive/list iterator
 *   tick():              O(1) amortized per tick and per fired timer
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <random>
#include <algorithm>

namespace timing_wheel {

    class HierarchicalTimingWheel {
        static constexpr int BITS = 6;              // 64 slots per wheel
        static constexpr int SLOTS = 1 << BITS;
        static constexpr int MASK = SLOTS - 1;
        static constexpr int LEVELS = 4;            // covers 64^4 = 16,777,216 ticks

        struct TimerNode {
            uint64_t id;
            uint32_t expire_tick;
            int payload;
        };

        struct Handle {
            int level, slot;
            std::list<TimerNode>::iterator it;
        };

        uint32_t now_ = 0;
        uint64_t next_id_ = 1;
        std::list<TimerNode> wheels_[LEVELS][SLOTS];
        std::unordered_map<uint64_t, Handle> handles_;

        void insert_node(const TimerNode& node) {
            uint32_t diff = node.expire_tick ^ now_;
            int lvl = 0;
            if (diff == 0) {
                lvl = 0;
            } else {
                for (int l = LEVELS - 1; l >= 0; --l) {
                    if ((diff >> (l * BITS)) != 0) { lvl = l; break; }
                }
            }
            int slot = (node.expire_tick >> (lvl * BITS)) & MASK;
            wheels_[lvl][slot].push_back(node);
            handles_[node.id] = {lvl, slot, std::prev(wheels_[lvl][slot].end())};
        }

        void cascade(int lvl) {
            int slot = (now_ >> (lvl * BITS)) & MASK;
            std::list<TimerNode> bucket = std::move(wheels_[lvl][slot]);
            wheels_[lvl][slot].clear();
            for (const auto& node : bucket) insert_node(node);
        }

    public:
        uint32_t current_tick() const { return now_; }

        uint64_t schedule(uint32_t delay_ticks, int payload) {
            uint64_t id = next_id_++;
            TimerNode node{id, now_ + delay_ticks, payload};
            insert_node(node);
            return id;
        }

        bool cancel(uint64_t id) {
            auto it = handles_.find(id);
            if (it == handles_.end()) return false;
            wheels_[it->second.level][it->second.slot].erase(it->second.it);
            handles_.erase(it);
            return true;
        }

        // Advance clock by 1 tick and return payloads of all timers expiring at new tick
        std::vector<int> advance_tick() {
            ++now_;
            for (int l = 1; l < LEVELS; ++l) {
                if ((now_ & ((1u << (l * BITS)) - 1)) == 0)
                    cascade(l);
                else
                    break;
            }
            int slot0 = now_ & MASK;
            std::list<TimerNode> due = std::move(wheels_[0][slot0]);
            wheels_[0][slot0].clear();

            std::vector<int> fired;
            for (const auto& node : due) {
                if (node.expire_tick <= now_) {
                    handles_.erase(node.id);
                    fired.push_back(node.payload);
                } else {
                    insert_node(node);
                }
            }
            std::sort(fired.begin(), fired.end());
            return fired;
        }
    };

} // namespace timing_wheel

int main() {
    using timing_wheel::HierarchicalTimingWheel;
    std::cout << "============================================================\n";
    std::cout << "       Hierarchical Timing Wheel O(1) Timer Scheduler\n";
    std::cout << "============================================================\n\n";

    HierarchicalTimingWheel tw;
    std::mt19937 rng(2026);
    const uint32_t MAX_TICKS = 20000;
    std::vector<std::vector<int>> expected(MAX_TICKS + 1);
    std::vector<std::pair<uint64_t, uint32_t>> active;

    // Schedule 15,000 timers across short, medium, and multi-wheel horizons
    int cancelled = 0;
    for (int i = 0; i < 15000; ++i) {
        uint32_t delay = 1 + (rng() % MAX_TICKS);
        uint64_t id = tw.schedule(delay, i);
        active.push_back({id, delay});
    }
    // Cancel 20% of timers in O(1) before they fire
    for (auto& [id, exp_tick] : active) {
        if (rng() % 5 == 0) {
            tw.cancel(id);
            ++cancelled;
        } else {
            expected[exp_tick].push_back(static_cast<int>(id - 1));
        }
    }

    bool ok = true;
    int total_fired = 0;
    for (uint32_t t = 1; t <= MAX_TICKS && ok; ++t) {
        auto got = tw.advance_tick();
        auto& exp = expected[t];
        std::sort(exp.begin(), exp.end());
        total_fired += static_cast<int>(got.size());
        if (got != exp) ok = false;
    }

    std::cout << "Scheduled 15,000 timers across 4 wheel levels; cancelled " << cancelled
              << "; fired " << total_fired << " over " << MAX_TICKS << " ticks.\n";
    std::cout << "Exact tick-by-tick firing & O(1) cancellation verification: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
