/*
 * ============================================================
 *       LSM-Tree (Log-Structured Merge-Tree) Storage Engine
 * ============================================================
 *
 * Description:
 *   An LSM-Tree (O'Neil et al., 1996) is the write-optimized
 *   storage engine architecture powering LevelDB, RocksDB,
 *   Apache Cassandra, ScyllaDB, and CockroachDB/Pebble:
 *
 *     1. MemTable: All incoming `put(key, val)` and `del(key)`
 *        (written as a tombstone marker) go into an in-memory
 *        sorted table (`std::map`).
 *     2. Flush to SSTable (Sorted String Table): When the MemTable
 *        reaches capacity, it is frozen and flushed into an immutable
 *        sorted SSTable run at Level 0, equipped with:
 *          - A 64-bit Bloom Filter for O(1) negative key skipping
 *          - A Sparse Fence-Pointer Index (every B-th key) to bound
 *            binary search to a single small block.
 *     3. Read Path (`get(key)`): Checks MemTable first, then searches
 *        SSTables from newest to oldest (skipping runs whose Bloom
 *        filter or [min_key, max_key] range excludes the key).
 *     4. Multi-Way Merge Compaction: When a level accumulates too
 *        many SSTables, a priority-queue K-way merge combines them
 *        into a single deduplicated SSTable at the next level,
 *        keeping only the latest sequence number per key and
 *        purging obsolete values and tombstones at the bottom level.
 *
 * Complexity:
 *   Write (put / del): O(log M) in-memory + sequential compaction
 *   Point read (get):  O(log M + L * log(BlockSize)) with Bloom filter
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <cstdint>
#include <random>
#include <algorithm>

namespace lsm_engine {

    struct Record {
        std::string key;
        std::string value;
        uint64_t seq;
        bool is_tombstone;
    };

    class SSTable {
        static constexpr size_t SPARSE_STEP = 16;

        std::vector<Record> records_;
        std::vector<std::pair<std::string, size_t>> sparse_index_;
        std::vector<uint64_t> bloom_;

        static uint64_t hash64(const std::string& s, uint64_t seed) {
            uint64_t h = 14695981039346656037ULL ^ seed;
            for (unsigned char c : s) { h ^= c; h *= 1099511628211ULL; }
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccdULL;
            h ^= h >> 33;
            return h;
        }

        void bloom_add(const std::string& k) {
            size_t bits = bloom_.size() * 64;
            for (uint64_t s : {0x1234ULL, 0x5678ULL, 0x9ABCULL}) {
                size_t b = static_cast<size_t>(hash64(k, s) % bits);
                bloom_[b >> 6] |= (1ULL << (b & 63));
            }
        }

        bool bloom_may_contain(const std::string& k) const {
            size_t bits = bloom_.size() * 64;
            for (uint64_t s : {0x1234ULL, 0x5678ULL, 0x9ABCULL}) {
                size_t b = static_cast<size_t>(hash64(k, s) % bits);
                if ((bloom_[b >> 6] & (1ULL << (b & 63))) == 0) return false;
            }
            return true;
        }

    public:
        mutable uint64_t bloom_skips = 0;

        explicit SSTable(std::vector<Record> sorted_records)
            : records_(std::move(sorted_records)) {
            size_t words = std::max<size_t>(2, (records_.size() * 12 + 63) / 64);
            bloom_.assign(words, 0ULL);
            for (size_t i = 0; i < records_.size(); ++i) {
                bloom_add(records_[i].key);
                if (i % SPARSE_STEP == 0)
                    sparse_index_.push_back({records_[i].key, i});
            }
        }

        const std::vector<Record>& records() const { return records_; }
        bool empty() const { return records_.empty(); }

        bool lookup(const std::string& key, Record& out) const {
            if (records_.empty() || key < records_.front().key || key > records_.back().key)
                return false;
            if (!bloom_may_contain(key)) {
                ++bloom_skips;
                return false;
            }
            // Binary search sparse fence pointers to narrow down to a 16-entry block
            auto it = std::upper_bound(
                sparse_index_.begin(), sparse_index_.end(), key,
                [](const std::string& k, const auto& entry) { return k < entry.first; });
            size_t lo = (it == sparse_index_.begin()) ? 0 : std::prev(it)->second;
            size_t hi = (it == sparse_index_.end()) ? records_.size() : it->second;

            auto rec_it = std::lower_bound(
                records_.begin() + static_cast<ptrdiff_t>(lo),
                records_.begin() + static_cast<ptrdiff_t>(hi), key,
                [](const Record& r, const std::string& k) { return r.key < k; });
            if (rec_it != records_.begin() + static_cast<ptrdiff_t>(hi) && rec_it->key == key) {
                out = *rec_it;
                return true;
            }
            return false;
        }
    };

    class LSMTree {
        size_t memtable_limit_;
        size_t level_fanout_;
        uint64_t seq_ = 0;
        std::map<std::string, Record> memtable_;
        std::vector<std::vector<SSTable>> levels_; // levels_[l] holds SSTables from oldest to newest

        // K-way merge of all SSTables in `runs`; if drop_tombstones is true, purges deleted keys
        static SSTable k_way_merge(const std::vector<SSTable>& runs, bool drop_tombstones) {
            struct Cursor {
                size_t run_idx, rec_idx;
                const Record* rec;
                // Min-heap by key ascending, then sequence number DESCENDING (newest first)
                bool operator>(const Cursor& o) const {
                    if (rec->key != o.rec->key) return rec->key > o.rec->key;
                    return rec->seq < o.rec->seq;
                }
            };
            std::priority_queue<Cursor, std::vector<Cursor>, std::greater<Cursor>> pq;
            for (size_t r = 0; r < runs.size(); ++r)
                if (!runs[r].empty())
                    pq.push({r, 0, &runs[r].records()[0]});

            std::vector<Record> merged;
            while (!pq.empty()) {
                Cursor top = pq.top(); pq.pop();
                if (top.rec_idx + 1 < runs[top.run_idx].records().size())
                    pq.push({top.run_idx, top.rec_idx + 1, &runs[top.run_idx].records()[top.rec_idx + 1]});

                if (merged.empty() || merged.back().key != top.rec->key) {
                    merged.push_back(*top.rec);
                }
            }
            if (drop_tombstones) {
                merged.erase(
                    std::remove_if(merged.begin(), merged.end(),
                                   [](const Record& r) { return r.is_tombstone; }),
                    merged.end());
            }
            return SSTable(std::move(merged));
        }

        void compact_if_needed() {
            for (size_t l = 0; l < levels_.size(); ++l) {
                if (levels_[l].size() < level_fanout_) break;
                if (l + 1 == levels_.size()) levels_.emplace_back();
                bool is_bottom = (l + 1 == levels_.size() - 1) && levels_[l + 1].empty();
                SSTable compacted = k_way_merge(levels_[l], is_bottom);
                levels_[l].clear();
                if (!compacted.empty()) levels_[l + 1].push_back(std::move(compacted));
            }
        }

    public:
        explicit LSMTree(size_t memtable_limit = 64, size_t fanout = 4)
            : memtable_limit_(memtable_limit), level_fanout_(fanout), levels_(1) {}

        void flush() {
            if (memtable_.empty()) return;
            std::vector<Record> recs;
            recs.reserve(memtable_.size());
            for (const auto& [k, r] : memtable_) recs.push_back(r);
            memtable_.clear();
            levels_[0].push_back(SSTable(std::move(recs)));
            compact_if_needed();
        }

        void put(const std::string& key, const std::string& value) {
            memtable_[key] = {key, value, ++seq_, false};
            if (memtable_.size() >= memtable_limit_) flush();
        }

        void del(const std::string& key) {
            memtable_[key] = {key, "", ++seq_, true};
            if (memtable_.size() >= memtable_limit_) flush();
        }

        bool get(const std::string& key, std::string& out) const {
            auto it = memtable_.find(key);
            if (it != memtable_.end()) {
                if (it->second.is_tombstone) return false;
                out = it->second.value;
                return true;
            }
            for (const auto& lvl : levels_) {
                for (int i = static_cast<int>(lvl.size()) - 1; i >= 0; --i) {
                    Record r;
                    if (lvl[i].lookup(key, r)) {
                        if (r.is_tombstone) return false;
                        out = r.value;
                        return true;
                    }
                }
            }
            return false;
        }

        uint64_t total_bloom_skips() const {
            uint64_t s = 0;
            for (const auto& lvl : levels_)
                for (const auto& sst : lvl) s += sst.bloom_skips;
            return s;
        }

        size_t num_levels() const { return levels_.size(); }
    };

} // namespace lsm_engine

int main() {
    using lsm_engine::LSMTree;
    std::cout << "============================================================\n";
    std::cout << "       LSM-Tree (Log-Structured Merge-Tree) Storage Engine\n";
    std::cout << "============================================================\n\n";

    LSMTree db(128, 4);
    std::map<std::string, std::string> oracle;
    std::mt19937 rng(2026);
    bool ok = true;

    for (int op = 0; op < 60000 && ok; ++op) {
        std::string key = "key_" + std::to_string(rng() % 5000);
        int act = rng() % 10;
        if (act < 5) {
            std::string val = "val_" + std::to_string(op);
            db.put(key, val);
            oracle[key] = val;
        } else if (act < 7) {
            db.del(key);
            oracle.erase(key);
        } else {
            std::string got;
            bool f1 = db.get(key, got);
            auto it = oracle.find(key);
            bool f2 = (it != oracle.end());
            if (f1 != f2 || (f1 && got != it->second)) ok = false;
        }
    }

    // Final full-keyspace scan after flushing remaining MemTable
    db.flush();
    for (int k = 0; k < 6000 && ok; ++k) {
        std::string key = "key_" + std::to_string(k);
        std::string got;
        bool f1 = db.get(key, got);
        auto it = oracle.find(key);
        bool f2 = (it != oracle.end());
        if (f1 != f2 || (f1 && got != it->second)) ok = false;
    }

    std::cout << "Completed 60,000 put/del/get ops across " << db.num_levels()
              << " compacted SSTable levels.\n";
    std::cout << "Bloom filters skipped " << db.total_bloom_skips()
              << " unnecessary SSTable binary searches.\n";
    std::cout << "Consistency against std::map oracle (100% keyspace match): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
