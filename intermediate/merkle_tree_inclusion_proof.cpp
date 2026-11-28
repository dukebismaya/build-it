/*
 * ============================================================
 *       Merkle Tree with O(log N) Inclusion & Tamper Proofs
 * ============================================================
 *
 * Description:
 *   A Merkle Tree (Ralph Merkle, 1979) is a binary hash tree used
 *   in Git, BitTorrent, ZFS/Btrfs, Cassandra anti-entropy repair,
 *   and Certificate Transparency logs to verify the integrity of
 *   any data block in O(log N) time using only the 128/256-bit
 *   root hash:
 *     - Leaf i stores H(0x00 || data_i).
 *     - Internal node u stores H(0x01 || left_hash || right_hash)
 *       (domain separation 0x00 vs 0x01 prevents second-preimage
 *       leaf/internal confusion attacks).
 *     - An Audit / Inclusion Proof for leaf i consists of the
 *       O(log N) sibling hashes along the path from leaf i to the
 *       root. Any client holding only the trusted root hash can
 *       recompute the path and authenticate leaf i in O(log N).
 *     - Updating a single data block recomputes only O(log N)
 *       hashes along its root path.
 *
 * Complexity:
 *   Build:                       O(N)
 *   Generate / Verify Proof:     O(log N)
 *   Update single leaf:          O(log N)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>

namespace merkle {

    struct Hash128 {
        uint64_t hi = 0, lo = 0;
        bool operator==(const Hash128& o) const { return hi == o.hi && lo == o.lo; }
        bool operator!=(const Hash128& o) const { return !(*this == o); }

        std::string hex() const {
            std::ostringstream os;
            os << std::hex << std::setfill('0') << std::setw(16) << hi << std::setw(16) << lo;
            return os.str();
        }
    };

    // High-avalanche 128-bit SipHash/Murmur-inspired mixing function for self-contained hashing
    uint64_t avalanche(uint64_t h) {
        h ^= h >> 33;
        h *= 0xff51afd7ed558ccdULL;
        h ^= h >> 33;
        h *= 0xc4ceb9fe1a85ec53ULL;
        h ^= h >> 33;
        return h;
    }

    Hash128 hash_bytes(uint8_t domain_prefix, const uint8_t* data, size_t len) {
        uint64_t h1 = 0x6a09e667f3bcc908ULL ^ domain_prefix;
        uint64_t h2 = 0xbb67ae8584caa73bULL ^ (static_cast<uint64_t>(domain_prefix) << 32);
        for (size_t i = 0; i < len; ++i) {
            h1 = avalanche(h1 ^ (static_cast<uint64_t>(data[i]) + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h2 >> 2)));
            h2 = avalanche(h2 ^ (static_cast<uint64_t>(data[i]) * 0xbf58476d1ce4e5b9ULL + h1));
        }
        return {avalanche(h1 ^ len), avalanche(h2 ^ (len * 0x94d049bb133111ebULL))};
    }

    Hash128 hash_leaf(const std::string& s) {
        return hash_bytes(0x00, reinterpret_cast<const uint8_t*>(s.data()), s.size());
    }

    Hash128 hash_internal(const Hash128& left, const Hash128& right) {
        uint64_t buf[4] = {left.hi, left.lo, right.hi, right.lo};
        return hash_bytes(0x01, reinterpret_cast<const uint8_t*>(buf), sizeof(buf));
    }

    struct ProofStep {
        Hash128 sibling;
        bool sibling_is_left;
    };

    class MerkleTree {
        size_t n_leaves_;
        size_t cap_;
        std::vector<Hash128> tree_; // 1-indexed complete binary tree of size 2 * cap_

    public:
        explicit MerkleTree(const std::vector<std::string>& blocks)
            : n_leaves_(blocks.size()) {
            cap_ = 1;
            while (cap_ < n_leaves_) cap_ <<= 1;
            tree_.assign(2 * cap_, Hash128{});
            for (size_t i = 0; i < n_leaves_; ++i)
                tree_[cap_ + i] = hash_leaf(blocks[i]);
            for (size_t i = n_leaves_; i < cap_; ++i)
                tree_[cap_ + i] = hash_leaf("");
            for (size_t u = cap_ - 1; u >= 1; --u)
                tree_[u] = hash_internal(tree_[2 * u], tree_[2 * u + 1]);
        }

        Hash128 root_hash() const { return tree_[1]; }

        void update_leaf(size_t index, const std::string& new_data) {
            size_t u = cap_ + index;
            tree_[u] = hash_leaf(new_data);
            for (u >>= 1; u >= 1; u >>= 1)
                tree_[u] = hash_internal(tree_[2 * u], tree_[2 * u + 1]);
        }

        std::vector<ProofStep> generate_proof(size_t index) const {
            std::vector<ProofStep> proof;
            for (size_t u = cap_ + index; u > 1; u >>= 1) {
                bool is_right_child = (u & 1) != 0;
                size_t sib = u ^ 1;
                proof.push_back({tree_[sib], is_right_child});
            }
            return proof;
        }

        static bool verify_proof(const std::string& leaf_data,
                                 const std::vector<ProofStep>& proof,
                                 const Hash128& expected_root) {
            Hash128 cur = hash_leaf(leaf_data);
            for (const auto& step : proof) {
                if (step.sibling_is_left) cur = hash_internal(step.sibling, cur);
                else                      cur = hash_internal(cur, step.sibling);
            }
            return cur == expected_root;
        }
    };

} // namespace merkle

int main() {
    using namespace merkle;
    std::cout << "============================================================\n";
    std::cout << "       Merkle Tree with O(log N) Inclusion & Tamper Proofs\n";
    std::cout << "============================================================\n\n";

    const int N = 1024;
    std::vector<std::string> blocks(N);
    for (int i = 0; i < N; ++i)
        blocks[i] = "tx_block_#" + std::to_string(i) + ":amount=" + std::to_string(i * 100);

    MerkleTree mt(blocks);
    std::cout << "Built Merkle tree over " << N << " blocks.\n";
    std::cout << "Root hash = " << mt.root_hash().hex() << "\n";

    auto sample_proof = mt.generate_proof(37);
    std::cout << "Inclusion proof for block #37 has " << sample_proof.size()
              << " sibling hashes (log2(1024) = 10).\n";

    bool ok = true;
    for (int i = 0; i < N && ok; ++i) {
        auto proof = mt.generate_proof(i);
        // Valid block must pass
        if (!MerkleTree::verify_proof(blocks[i], proof, mt.root_hash())) ok = false;
        // Tampered block data must be rejected
        if (MerkleTree::verify_proof(blocks[i] + "!", proof, mt.root_hash())) ok = false;
        // Tampered sibling hash must be rejected
        auto bad_proof = proof;
        bad_proof[i % bad_proof.size()].sibling.lo ^= 1ULL;
        if (MerkleTree::verify_proof(blocks[i], bad_proof, mt.root_hash())) ok = false;
    }

    // Test O(log N) incremental leaf update against a freshly built MerkleTree
    blocks[511] = "tx_block_#511:amount=9999999";
    mt.update_leaf(511, blocks[511]);
    MerkleTree mt_rebuilt(blocks);
    if (mt.root_hash() != mt_rebuilt.root_hash()) ok = false;

    std::cout << "\n1,024 inclusion proofs, tamper detection & O(log N) leaf update: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
