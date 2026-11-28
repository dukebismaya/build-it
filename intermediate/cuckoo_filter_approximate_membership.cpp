/*
 * ============================================================
 *       Cuckoo Filter (Deletable Approximate Set Membership)
 * ============================================================
 *
 * Description:
 *   A Cuckoo Filter (Fan, Andersen, Kaminsky, Mitzenmacher,
 *   CoNEXT 2014) is a compact probabilistic data structure for
 *   set membership that improves upon Bloom Filters in three ways:
 *     1. Supports O(1) DELETION of items without false negatives.
 *     2. Higher lookup locality (at most 2 cache-line bucket reads).
 *     3. Lower space overhead at low false-positive rates (< 3%).
 *
 *   Partial-Key Cuckoo Hashing:
 *     - Each item x is mapped to a small f-bit fingerprint
 *       fp = fingerprint(x) != 0 and primary bucket b1 = h1(x).
 *     - Its alternate bucket b2 is computed from ONLY b1 and fp
 *       (without storing the full key x!):
 *         b2 = b1 ^ h2(fp)
 *       Because XOR is an involution, b1 = b2 ^ h2(fp), allowing
 *       cuckoo kicks to relocate existing fingerprints between
 *       their two candidate buckets during insertion!
 *
 * Complexity:
 *   contains / erase: O(1) worst-case (checks 2 buckets of 4 slots)
 *   insert:           O(1) amortized
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <array>
#include <cstdint>
#include <random>
#include <iomanip>
#include <utility>

namespace cuckoo_filter {

    class CuckooFilter {
        static constexpr int BUCKET_SIZE = 4;
        static constexpr int MAX_KICKS = 500;

        struct Bucket {
            std::array<uint16_t, BUCKET_SIZE> slots{};
        };

        size_t num_buckets_;
        size_t mask_;
        size_t count_ = 0;
        std::vector<Bucket> buckets_;
        mutable std::mt19937 rng_{1337};

        static uint64_t mix64(uint64_t k) {
            k ^= k >> 33;
            k *= 0xff51afd7ed558ccdULL;
            k ^= k >> 33;
            k *= 0xc4ceb9fe1a85ec53ULL;
            k ^= k >> 33;
            return k;
        }

        static uint16_t fingerprint_of(uint64_t h) {
            uint16_t fp = static_cast<uint16_t>((h >> 32) & 0xFFFFu);
            return fp == 0 ? 1 : fp; // reserve 0 for empty slot
        }

        size_t alt_index(size_t b, uint16_t fp) const {
            return (b ^ static_cast<size_t>(mix64(fp))) & mask_;
        }

        static bool insert_into_bucket(Bucket& b, uint16_t fp) {
            for (auto& s : b.slots) {
                if (s == 0) { s = fp; return true; }
            }
            return false;
        }

        static bool bucket_contains(const Bucket& b, uint16_t fp) {
            for (uint16_t s : b.slots) if (s == fp) return true;
            return false;
        }

        static bool remove_from_bucket(Bucket& b, uint16_t fp) {
            for (auto& s : b.slots) {
                if (s == fp) { s = 0; return true; }
            }
            return false;
        }

    public:
        explicit CuckooFilter(size_t max_items) {
            size_t b = 1;
            while (b * BUCKET_SIZE < (max_items * 10) / 9 + 4) b <<= 1;
            num_buckets_ = b;
            mask_ = b - 1;
            buckets_.resize(num_buckets_);
        }

        bool insert(uint64_t key) {
            uint64_t h = mix64(key);
            uint16_t fp = fingerprint_of(h);
            size_t i1 = static_cast<size_t>(h) & mask_;
            size_t i2 = alt_index(i1, fp);

            if (insert_into_bucket(buckets_[i1], fp) || insert_into_bucket(buckets_[i2], fp)) {
                ++count_;
                return true;
            }
            size_t i = (rng_() & 1) ? i1 : i2;
            for (int n = 0; n < MAX_KICKS; ++n) {
                int slot = rng_() % BUCKET_SIZE;
                std::swap(fp, buckets_[i].slots[slot]);
                i = alt_index(i, fp);
                if (insert_into_bucket(buckets_[i], fp)) {
                    ++count_;
                    return true;
                }
            }
            return false;
        }

        bool contains(uint64_t key) const {
            uint64_t h = mix64(key);
            uint16_t fp = fingerprint_of(h);
            size_t i1 = static_cast<size_t>(h) & mask_;
            size_t i2 = alt_index(i1, fp);
            return bucket_contains(buckets_[i1], fp) || bucket_contains(buckets_[i2], fp);
        }

        bool erase(uint64_t key) {
            uint64_t h = mix64(key);
            uint16_t fp = fingerprint_of(h);
            size_t i1 = static_cast<size_t>(h) & mask_;
            size_t i2 = alt_index(i1, fp);
            if (remove_from_bucket(buckets_[i1], fp) || remove_from_bucket(buckets_[i2], fp)) {
                --count_;
                return true;
            }
            return false;
        }

        size_t size() const { return count_; }
        double load_factor() const {
            return static_cast<double>(count_) / (num_buckets_ * BUCKET_SIZE);
        }
    };

} // namespace cuckoo_filter

int main() {
    using cuckoo_filter::CuckooFilter;
    std::cout << "============================================================\n";
    std::cout << "       Cuckoo Filter (Deletable Approximate Set Membership)\n";
    std::cout << "============================================================\n\n";

    const int N = 12000;
    CuckooFilter cf(N);

    bool ok = true;
    for (int i = 0; i < N; ++i)
        if (!cf.insert(static_cast<uint64_t>(i))) ok = false;

    std::cout << "Inserted " << cf.size() << " elements (load factor = "
              << std::fixed << std::setprecision(2) << (100.0 * cf.load_factor()) << "%)\n";

    // Zero false negatives check
    for (int i = 0; i < N; ++i)
        if (!cf.contains(static_cast<uint64_t>(i))) ok = false;

    // Measure false-positive rate over 100,000 unseen keys (expected ~ 8 / 65536 = 0.012%)
    int fp_count = 0;
    const int QUERIES = 100000;
    for (int i = N; i < N + QUERIES; ++i)
        if (cf.contains(static_cast<uint64_t>(i))) ++fp_count;
    double fp_rate = static_cast<double>(fp_count) / QUERIES;
    std::cout << "False positive rate on 100,000 non-member keys: "
              << std::setprecision(4) << (100.0 * fp_rate) << "% (" << fp_count << " / " << QUERIES << ")\n";

    // Delete first half [0 .. N/2 - 1] and verify they are gone while [N/2 .. N - 1] remain 100% present
    for (int i = 0; i < N / 2; ++i)
        if (!cf.erase(static_cast<uint64_t>(i))) ok = false;
    for (int i = 0; i < N / 2; ++i)
        if (cf.contains(static_cast<uint64_t>(i))) ok = false;
    for (int i = N / 2; i < N; ++i)
        if (!cf.contains(static_cast<uint64_t>(i))) ok = false;

    if (fp_rate > 0.005) ok = false;
    std::cout << "\nZero false negatives, <0.5% false positives & exact deletion check: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
