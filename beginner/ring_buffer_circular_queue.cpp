/*
 * ============================================================
 *            Ring Buffer (Power-of-Two Circular Queue)
 * ============================================================
 *
 * Description:
 *   A fixed-capacity ring buffer (circular queue) backed by a
 *   contiguous array whose capacity is rounded up to a power of 2.
 *   Using monotonically increasing 64-bit read/write cursors
 *   (head and tail) and bitwise AND masking (index = cursor & mask):
 *     - Full and empty states are distinguished without wasting
 *       a slot (empty: write_pos == read_pos;
 *               full:  write_pos - read_pos == capacity).
 *     - Modulo wrap-around is replaced by a single bitwise AND.
 *     - Supports both strict (reject when full) and telemetry /
 *       audio streaming (overwrite oldest when full) modes, as
 *       well as bulk write/read.
 *
 * Complexity:
 *   push / pop / push_overwrite / operator[]: O(1)
 *   Space: O(capacity) preallocated once (zero heap allocations
 *          during steady-state operation).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <deque>
#include <random>
#include <algorithm>

namespace ring_buffer {

    template <typename T>
    class RingBuffer {
        std::vector<T> buf_;
        size_t cap_;
        size_t mask_;
        uint64_t read_pos_ = 0;
        uint64_t write_pos_ = 0;

        static size_t next_pow2(size_t n) {
            size_t p = 1;
            while (p < n) p <<= 1;
            return p;
        }

    public:
        explicit RingBuffer(size_t min_capacity)
            : cap_(next_pow2(std::max<size_t>(1, min_capacity))),
              mask_(cap_ - 1) {
            buf_.resize(cap_);
        }

        size_t capacity() const { return cap_; }
        size_t size() const { return static_cast<size_t>(write_pos_ - read_pos_); }
        bool empty() const { return write_pos_ == read_pos_; }
        bool full() const { return size() == cap_; }

        // Strict push: returns false if buffer is full
        bool try_push(const T& item) {
            if (full()) return false;
            buf_[write_pos_ & mask_] = item;
            ++write_pos_;
            return true;
        }

        // Streaming push: overwrites the oldest element if full; returns true if overwritten
        bool push_overwrite(const T& item) {
            bool overwritten = full();
            if (overwritten) ++read_pos_;
            buf_[write_pos_ & mask_] = item;
            ++write_pos_;
            return overwritten;
        }

        bool try_pop(T& out) {
            if (empty()) return false;
            out = buf_[read_pos_ & mask_];
            ++read_pos_;
            return true;
        }

        // Random access: i = 0 is oldest element, i = size()-1 is newest
        const T& operator[](size_t i) const {
            return buf_[(read_pos_ + i) & mask_];
        }

        size_t write_bulk(const T* data, size_t count) {
            size_t avail = cap_ - size();
            size_t n = std::min(count, avail);
            for (size_t i = 0; i < n; ++i)
                buf_[(write_pos_ + i) & mask_] = data[i];
            write_pos_ += n;
            return n;
        }

        size_t read_bulk(T* out, size_t count) {
            size_t n = std::min(count, size());
            for (size_t i = 0; i < n; ++i)
                out[i] = buf_[(read_pos_ + i) & mask_];
            read_pos_ += n;
            return n;
        }
    };

} // namespace ring_buffer

int main() {
    using ring_buffer::RingBuffer;
    std::cout << "============================================================\n";
    std::cout << "            Ring Buffer (Power-of-Two Circular Queue)\n";
    std::cout << "============================================================\n\n";

    RingBuffer<int> rb(8);
    std::cout << "Capacity = " << rb.capacity() << "\n";
    for (int i = 1; i <= 11; ++i) {
        bool dropped = rb.push_overwrite(i * 10);
        std::cout << "push_overwrite(" << i * 10 << ") -> size=" << rb.size()
                  << (dropped ? " [overwrote oldest]" : "") << "\n";
    }
    std::cout << "Current window (oldest to newest): ";
    for (size_t i = 0; i < rb.size(); ++i) std::cout << rb[i] << " ";
    std::cout << "\n";

    std::mt19937 rng(42);
    RingBuffer<int> q(64);
    std::deque<int> ref;
    bool ok = true;
    for (int step = 0; step < 200000 && ok; ++step) {
        int action = rng() % 10;
        if (action < 4) {
            int v = static_cast<int>(rng() % 10000);
            bool got = q.try_push(v);
            bool exp = ref.size() < 64;
            if (got != exp) ok = false;
            if (exp) ref.push_back(v);
        } else if (action < 7) {
            int out = -1;
            bool got = q.try_pop(out);
            bool exp = !ref.empty();
            if (got != exp) ok = false;
            if (exp) {
                if (out != ref.front()) ok = false;
                ref.pop_front();
            }
        } else {
            int v = static_cast<int>(rng() % 10000);
            q.push_overwrite(v);
            if (ref.size() == 64) ref.pop_front();
            ref.push_back(v);
        }
        if (q.size() != ref.size()) ok = false;
        if (!ref.empty() && (q[0] != ref.front() || q[q.size() - 1] != ref.back())) ok = false;
    }

    std::cout << "\nRandom test vs std::deque (200000 push/pop/overwrite ops): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
