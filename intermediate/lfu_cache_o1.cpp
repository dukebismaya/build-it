/*
 * ============================================================
 *         O(1) LFU Cache (Least Frequently Used Cache)
 * ============================================================
 *
 * Description:
 *   An LFU Cache evicts the key with the lowest access frequency,
 *   breaking ties by evicting the Least Recently Used (LRU) key
 *   among those sharing the minimum frequency.
 *
 *   To achieve strict O(1) time for both get(key) and put(key, val)
 *   (Shah, Mitra, Matani 2010 / LeetCode 460):
 *     - freq_buckets[f] is a doubly linked list (std::list<Entry>)
 *       holding all cache items currently accessed exactly f times,
 *       ordered from most-recently used (front) to least-recently
 *       used (back).
 *     - key_map[key] stores an iterator directly into freq_buckets[f].
 *     - min_freq tracks the smallest non-empty frequency bucket:
 *         * On a new insertion, min_freq is reset to 1.
 *         * When touching an entry at frequency f, it is erased
 *           from freq_buckets[f] and pushed to freq_buckets[f + 1];
 *           if freq_buckets[min_freq] becomes empty, min_freq increments
 *           to f + 1 in O(1)!
 *
 * Complexity:
 *   get(key):       O(1) average
 *   put(key, val):  O(1) average
 *   Space:          O(capacity)
 * ============================================================
 */

#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
#include <random>

namespace lfu_cache {

    template <typename K, typename V>
    class LFUCache {
        struct Node {
            K key;
            V value;
            int freq;
        };

        size_t capacity_;
        int min_freq_ = 0;
        std::unordered_map<K, typename std::list<Node>::iterator> key_table_;
        std::unordered_map<int, std::list<Node>> freq_table_;

        void touch(typename std::list<Node>::iterator it) {
            Node item = *it;
            int f = item.freq;
            freq_table_[f].erase(it);
            if (freq_table_[f].empty()) {
                freq_table_.erase(f);
                if (min_freq_ == f) ++min_freq_;
            }
            item.freq = f + 1;
            freq_table_[f + 1].push_front(item);
            key_table_[item.key] = freq_table_[f + 1].begin();
        }

    public:
        explicit LFUCache(size_t capacity) : capacity_(capacity) {}

        bool get(const K& key, V& out) {
            auto it = key_table_.find(key);
            if (it == key_table_.end()) return false;
            out = it->second->value;
            touch(it->second);
            return true;
        }

        void put(const K& key, const V& value) {
            if (capacity_ == 0) return;
            auto it = key_table_.find(key);
            if (it != key_table_.end()) {
                it->second->value = value;
                touch(it->second);
                return;
            }
            if (key_table_.size() == capacity_) {
                auto& lru_list = freq_table_[min_freq_];
                K evict_key = lru_list.back().key;
                lru_list.pop_back();
                if (lru_list.empty()) freq_table_.erase(min_freq_);
                key_table_.erase(evict_key);
            }
            min_freq_ = 1;
            freq_table_[1].push_front({key, value, 1});
            key_table_[key] = freq_table_[1].begin();
        }

        size_t size() const { return key_table_.size(); }
    };

    // O(C) reference oracle for randomized verification
    class ReferenceLFU {
        struct Item { int key, val, freq, tick; };
        size_t cap;
        int clock = 0;
        std::vector<Item> items;

    public:
        explicit ReferenceLFU(size_t c) : cap(c) {}

        bool get(int k, int& out) {
            ++clock;
            for (auto& it : items) {
                if (it.key == k) {
                    ++it.freq;
                    it.tick = clock;
                    out = it.val;
                    return true;
                }
            }
            return false;
        }

        void put(int k, int v) {
            ++clock;
            if (cap == 0) return;
            for (auto& it : items) {
                if (it.key == k) {
                    it.val = v;
                    ++it.freq;
                    it.tick = clock;
                    return;
                }
            }
            if (items.size() == cap) {
                size_t victim = 0;
                for (size_t i = 1; i < items.size(); ++i) {
                    if (items[i].freq < items[victim].freq ||
                        (items[i].freq == items[victim].freq && items[i].tick < items[victim].tick))
                        victim = i;
                }
                items.erase(items.begin() + static_cast<ptrdiff_t>(victim));
            }
            items.push_back({k, v, 1, clock});
        }
    };

} // namespace lfu_cache

int main() {
    using namespace lfu_cache;
    std::cout << "============================================================\n";
    std::cout << "         O(1) LFU Cache (Least Frequently Used Cache)\n";
    std::cout << "============================================================\n\n";

    LFUCache<int, int> cache(2);
    int v = -1;
    cache.put(1, 10);
    cache.put(2, 20);
    cache.get(1, v);   // freq(1) = 2, freq(2) = 1
    cache.put(3, 30);  // evicts key 2 (freq 1)
    std::cout << "After put(1), put(2), get(1), put(3) on cap=2:\n";
    std::cout << "  get(2) found? " << (cache.get(2, v) ? "yes" : "no (evicted)") << "\n";
    std::cout << "  get(3) found? " << (cache.get(3, v) ? "yes, val=" + std::to_string(v) : "no") << "\n";
    cache.put(4, 40);  // freq(1)=2, freq(3)=2, key 1 is LRU among freq 2 => evicts 1
    std::cout << "After get(3), put(4) (tie-break by LRU among freq=2):\n";
    std::cout << "  get(1) found? " << (cache.get(1, v) ? "yes" : "no (evicted)") << "\n";
    std::cout << "  get(3) found? " << (cache.get(3, v) ? "yes, val=" + std::to_string(v) : "no") << "\n";

    std::mt19937 rng(2026);
    bool ok = true;
    for (size_t cap : {1, 3, 8, 25}) {
        LFUCache<int, int> fast(cap);
        ReferenceLFU ref(cap);
        for (int op = 0; op < 30000 && ok; ++op) {
            int key = rng() % static_cast<int>(cap * 3);
            if (rng() & 1) {
                int val = static_cast<int>(rng() % 10000);
                fast.put(key, val);
                ref.put(key, val);
            } else {
                int v1 = -1, v2 = -1;
                bool f1 = fast.get(key, v1);
                bool f2 = ref.get(key, v2);
                if (f1 != f2 || (f1 && v1 != v2)) ok = false;
            }
        }
    }
    std::cout << "\nRandom trace vs reference oracle (120000 get/put operations): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
