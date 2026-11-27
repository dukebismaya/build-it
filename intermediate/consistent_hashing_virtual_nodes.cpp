/*
 * ============================================================
 *          Consistent Hashing Ring with Virtual Nodes
 * ============================================================
 *
 * Description:
 *   Consistent hashing (Karger et al., 1997) partitions keys
 *   across a dynamic cluster of servers (Cassandra, DynamoDB,
 *   Memcached, Envoy load balancers) so that adding or removing
 *   a node among N nodes remaps only ~1/N of the keys instead
 *   of ~(N-1)/N with `hash(key) % N`.
 *
 *   Algorithm:
 *     - Map the hash space onto a 64-bit ring [0, 2^64 - 1].
 *     - Each physical server S is assigned V virtual nodes
 *       ("vnodes") hashed onto the ring at positions h(S#0 .. S#(V-1))
 *       stored in a sorted std::map<uint64_t, NodeId>.
 *     - To route a key K, compute h(K) and look up the first vnode
 *       clockwise via `ring.lower_bound(h(K))`, wrapping around to
 *       `ring.begin()` if past the end.
 *     - To replicate a key across R distinct physical servers,
 *       walk clockwise skipping duplicate physical nodes.
 *
 * Complexity:
 *   add_node / remove_node: O(V log(N * V))
 *   get_node(key):          O(log(N * V))
 *   Space:                  O(N * V)
 * ============================================================
 */

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <algorithm>

namespace consistent_hash {

    // Fast 64-bit FNV-1a + SplitMix64 avalanche finalizer
    uint64_t hash64(const std::string& s) {
        uint64_t h = 14695981039346656037ULL;
        for (unsigned char c : s) {
            h ^= c;
            h *= 1099511628211ULL;
        }
        h ^= h >> 30;
        h *= 0xbf58476d1ce4e5b9ULL;
        h ^= h >> 27;
        h *= 0x94d049bb133111ebULL;
        h ^= h >> 31;
        return h;
    }

    class ConsistentHashRing {
        int vnodes_per_server_;
        std::map<uint64_t, std::string> ring_;

    public:
        explicit ConsistentHashRing(int vnodes = 150) : vnodes_per_server_(vnodes) {}

        void add_node(const std::string& node_id) {
            for (int i = 0; i < vnodes_per_server_; ++i) {
                uint64_t pos = hash64(node_id + "#vnode#" + std::to_string(i));
                ring_[pos] = node_id;
            }
        }

        void remove_node(const std::string& node_id) {
            for (int i = 0; i < vnodes_per_server_; ++i) {
                uint64_t pos = hash64(node_id + "#vnode#" + std::to_string(i));
                auto it = ring_.find(pos);
                if (it != ring_.end() && it->second == node_id)
                    ring_.erase(it);
            }
        }

        std::string get_node(const std::string& key) const {
            if (ring_.empty()) return "";
            auto it = ring_.lower_bound(hash64(key));
            if (it == ring_.end()) it = ring_.begin();
            return it->second;
        }

        // Return up to replication_factor distinct physical servers clockwise
        std::vector<std::string> get_preference_list(const std::string& key, size_t replicas) const {
            std::vector<std::string> out;
            if (ring_.empty()) return out;
            auto start = ring_.lower_bound(hash64(key));
            if (start == ring_.end()) start = ring_.begin();
            auto it = start;
            do {
                if (std::find(out.begin(), out.end(), it->second) == out.end()) {
                    out.push_back(it->second);
                    if (out.size() == replicas) break;
                }
                ++it;
                if (it == ring_.end()) it = ring_.begin();
            } while (it != start);
            return out;
        }
    };

} // namespace consistent_hash

int main() {
    using namespace consistent_hash;
    std::cout << "============================================================\n";
    std::cout << "          Consistent Hashing Ring with Virtual Nodes\n";
    std::cout << "============================================================\n\n";

    ConsistentHashRing ring(200);
    std::vector<std::string> nodes = {"db-node-0", "db-node-1", "db-node-2", "db-node-3", "db-node-4"};
    for (const auto& n : nodes) ring.add_node(n);

    const int NUM_KEYS = 50000;
    std::map<std::string, int> counts;
    std::vector<std::string> before(NUM_KEYS);
    for (int i = 0; i < NUM_KEYS; ++i) {
        std::string k = "user:session:" + std::to_string(i);
        before[i] = ring.get_node(k);
        ++counts[before[i]];
    }

    std::cout << "Key distribution across 5 servers (50,000 keys, 200 vnodes/server):\n";
    double ideal = NUM_KEYS / 5.0, sq_err = 0.0;
    for (const auto& [node, c] : counts) {
        double pct = 100.0 * c / NUM_KEYS;
        sq_err += (c - ideal) * (c - ideal);
        std::cout << "  " << node << ": " << std::setw(6) << c
                  << " keys (" << std::fixed << std::setprecision(2) << pct << "%)\n";
    }
    double rel_stddev = std::sqrt(sq_err / 5.0) / ideal;

    // Add a 6th node and verify monotonic minimal disruption (~1/6 = 16.7% moved, and ONLY to db-node-5)
    ring.add_node("db-node-5");
    int moved = 0;
    bool monotonic = true;
    for (int i = 0; i < NUM_KEYS; ++i) {
        std::string after = ring.get_node("user:session:" + std::to_string(i));
        if (after != before[i]) {
            ++moved;
            if (after != "db-node-5") monotonic = false;
        }
    }
    double moved_frac = static_cast<double>(moved) / NUM_KEYS;
    std::cout << "\nAdded db-node-5: " << moved << " / " << NUM_KEYS << " keys remapped ("
              << std::setprecision(2) << (100.0 * moved_frac) << "%, ideal 1/6 = 16.67%)\n";

    auto replicas = ring.get_preference_list("order:987654", 3);
    std::cout << "3-way replica preference list for 'order:987654': ";
    for (const auto& r : replicas) std::cout << r << " ";
    std::cout << "\n";

    // Removing db-node-5 must restore 100% of keys to their exact original owners
    ring.remove_node("db-node-5");
    bool restored = true;
    for (int i = 0; i < NUM_KEYS; ++i)
        if (ring.get_node("user:session:" + std::to_string(i)) != before[i]) restored = false;

    bool ok = monotonic && restored && rel_stddev < 0.12 &&
              moved_frac > 0.13 && moved_frac < 0.21 && replicas.size() == 3;
    std::cout << "\nMonotonicity, load balance & exact restoration check: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
