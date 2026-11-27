/*
 * ============================================================
 *       Robin Hood Open-Addressing Hash Table
 * ============================================================
 *
 * Description:
 *   Robin Hood hashing (Pedro Celis, 1986) is a cache-friendly
 *   flat open-addressing hash table variant used in high-speed
 *   runtimes (e.g., Rust's original std::collections::HashMap):
 *
 *     - Each occupied slot tracks its Probe Sequence Length (PSL),
 *       also called "distance from initial bucket" (DIB).
 *     - On insertion, as we probe forward, if the incoming element's
 *       PSL exceeds the current resident's PSL ("rich" resident,
 *       "poor" newcomer), we SWAP them and continue inserting the
 *       displaced resident! This equalizes probe lengths and
 *       drastically reduces probe-length variance.
 *     - Lookup can terminate early as soon as the current slot's
 *       PSL is strictly smaller than our search PSL.
 *     - Deletion uses BACKWARD-SHIFT deletion instead of tombstones,
 *       shifting subsequent slots with PSL > 0 back by 1 so table
 *       performance never degrades over long insert/erase workloads.
 *
 * Complexity:
 *   insert / find / erase: O(1) expected, O(1) max probe variance
 *   Space:                 O(capacity) in a single flat array
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <random>
#include <utility>
#include <algorithm>

namespace robin_hood {

    template <typename K, typename V>
    class RobinHoodMap {
        struct Slot {
            K key{};
            V value{};
            int psl = -1; // -1 = empty, >= 0 = distance from ideal bucket
        };

        std::vector<Slot> table_;
        size_t size_ = 0;
        size_t mask_ = 0;

        static uint64_t mix64(uint64_t x) {
            x ^= x >> 30;
            x *= 0xbf58476d1ce4e5b9ULL;
            x ^= x >> 27;
            x *= 0x94d049bb133111ebULL;
            x ^= x >> 31;
            return x;
        }

        size_t bucket_of(const K& key) const {
            return static_cast<size_t>(mix64(static_cast<uint64_t>(std::hash<K>{}(key)))) & mask_;
        }

        void rehash(size_t new_cap) {
            std::vector<Slot> old = std::move(table_);
            table_.assign(new_cap, Slot{});
            mask_ = new_cap - 1;
            size_ = 0;
            for (const auto& s : old)
                if (s.psl >= 0) insert(s.key, s.value);
        }

    public:
        explicit RobinHoodMap(size_t init_cap = 16) {
            size_t cap = 16;
            while (cap < init_cap) cap <<= 1;
            table_.assign(cap, Slot{});
            mask_ = cap - 1;
        }

        size_t size() const { return size_; }

        void insert(K key, V value) {
            if ((size_ + 1) * 4 >= table_.size() * 3) // max load factor 0.75
                rehash(table_.size() * 2);

            size_t idx = bucket_of(key);
            int dist = 0;
            for (;;) {
                if (table_[idx].psl < 0) {
                    table_[idx] = {std::move(key), std::move(value), dist};
                    ++size_;
                    return;
                }
                if (table_[idx].key == key) {
                    table_[idx].value = std::move(value);
                    return;
                }
                if (dist > table_[idx].psl) {
                    std::swap(key, table_[idx].key);
                    std::swap(value, table_[idx].value);
                    std::swap(dist, table_[idx].psl);
                }
                idx = (idx + 1) & mask_;
                ++dist;
            }
        }

        bool find(const K& key, V& out) const {
            size_t idx = bucket_of(key);
            int dist = 0;
            for (;;) {
                if (table_[idx].psl < 0 || dist > table_[idx].psl)
                    return false; // early termination via Robin Hood invariant!
                if (table_[idx].key == key) {
                    out = table_[idx].value;
                    return true;
                }
                idx = (idx + 1) & mask_;
                ++dist;
            }
        }

        bool erase(const K& key) {
            size_t idx = bucket_of(key);
            int dist = 0;
            for (;;) {
                if (table_[idx].psl < 0 || dist > table_[idx].psl)
                    return false;
                if (table_[idx].key == key) break;
                idx = (idx + 1) & mask_;
                ++dist;
            }
            // Backward-shift deletion (no tombstones needed)
            size_t next = (idx + 1) & mask_;
            while (table_[next].psl > 0) {
                table_[idx] = std::move(table_[next]);
                --table_[idx].psl;
                idx = next;
                next = (idx + 1) & mask_;
            }
            table_[idx] = Slot{};
            --size_;
            return true;
        }

        int max_psl() const {
            int m = 0;
            for (const auto& s : table_) m = std::max(m, s.psl);
            return m;
        }
    };

} // namespace robin_hood

int main() {
    using robin_hood::RobinHoodMap;
    std::cout << "============================================================\n";
    std::cout << "          Robin Hood Open-Addressing Hash Table\n";
    std::cout << "============================================================\n\n";

    std::mt19937 rng(2026);
    RobinHoodMap<int, int> rh;
    std::unordered_map<int, int> ref;
    bool ok = true;

    for (int op = 0; op < 200000 && ok; ++op) {
        int k = static_cast<int>(rng() % 20000);
        int act = rng() % 10;
        if (act < 5) {
            int v = static_cast<int>(rng());
            rh.insert(k, v);
            ref[k] = v;
        } else if (act < 8) {
            int v = 0;
            bool f1 = rh.find(k, v);
            auto it = ref.find(k);
            bool f2 = (it != ref.end());
            if (f1 != f2 || (f1 && v != it->second)) ok = false;
        } else {
            bool e1 = rh.erase(k);
            bool e2 = (ref.erase(k) > 0);
            if (e1 != e2) ok = false;
        }
        if (rh.size() != ref.size()) ok = false;
    }

    std::cout << "Final table size = " << rh.size()
              << ", maximum probe sequence length (PSL) = " << rh.max_psl() << "\n";
    std::cout << "Random stress test vs std::unordered_map (200000 insert/find/erase ops): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
