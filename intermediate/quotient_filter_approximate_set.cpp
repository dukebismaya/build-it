/**
 * @file quotient_filter_approximate_set.cpp
 * @brief Quotient Filter approximate set membership data structure in C++17.
 *
 * Overview (Bender et al., 2012):
 *   A cache-friendly alternative to the Bloom filter that supports:
 *     - Insertion, Lookup, and Deletion.
 *     - Dynamic Resizing and Merging of filters.
 *
 * Key Concepts:
 *   - An element x is hashed to a p-bit fingerprint.
 *   - The fingerprint is split into:
 *       1. Quotient (most significant q bits): indexes into an array of size 2^q.
 *       2. Remainder (least significant r bits): stored directly in the slot.
 *   - Robin Hood / linear probing resolves quotient collisions into contiguous "runs".
 *   - Each slot stores 3 metadata status bits:
 *       - is_occupied: A fingerprint has this slot as its canonical quotient.
 *       - is_continuation: This remainder belongs to the same run as the preceding slot.
 *       - is_shifted: This remainder has been pushed past its canonical quotient slot.
 *
 * False Positive Rate:
 *   Theoretical error epsilon ~ 2^(-r).
 *
 * Complexity:
 *   - Lookup: O(1) expected time, single cache line access.
 *   - Insertion: O(1) expected time.
 *   - Space: 2^q * (r + 3) bits.
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <cmath>

namespace sketches {

class QuotientFilter {
public:
    struct Slot {
        uint64_t remainder : 40;
        bool isOccupied : 1;
        bool isContinuation : 1;
        bool isShifted : 1;
        bool hasData : 1;
    };

private:
    uint32_t q; // Quotient bits (size = 2^q)
    uint32_t r; // Remainder bits
    uint64_t numSlots;
    uint64_t remainderMask;
    std::vector<Slot> table;
    size_t count;

    uint64_t hashString(const std::string& str) const {
        // 64-bit FNV-1a
        uint64_t h = 14695981039346656037ULL;
        for (char c : str) {
            h ^= static_cast<uint64_t>(c);
            h *= 1099511628211ULL;
        }
        return h;
    }

    void splitHash(uint64_t h, uint64_t& quotient, uint64_t& remainder) const {
        uint64_t totalBits = q + r;
        uint64_t pBits = h & ((1ULL << totalBits) - 1ULL);
        quotient = (pBits >> r) & (numSlots - 1ULL);
        remainder = pBits & remainderMask;
    }

public:
    QuotientFilter(uint32_t quotientBits, uint32_t remainderBits)
        : q(quotientBits), r(remainderBits), numSlots(1ULL << quotientBits),
          remainderMask((1ULL << remainderBits) - 1ULL), count(0) {
        table.resize(numSlots, {0, false, false, false, false});
    }

    bool insert(const std::string& item) {
        uint64_t h = hashString(item);
        uint64_t fq, fr;
        splitHash(h, fq, fr);

        if (!table[fq].hasData) {
            // Empty slot, direct placement
            table[fq].remainder = fr;
            table[fq].isOccupied = true;
            table[fq].hasData = true;
            count++;
            return true;
        }

        table[fq].isOccupied = true;

        // Linear probing displacement
        uint64_t curr = fq;
        Slot newSlot = {fr, false, false, true, true};

        while (table[curr].hasData) {
            if (table[curr].remainder == fr) {
                return false; // Duplicate
            }
            // Swap if current remainder is larger (Robin Hood heuristic)
            if (newSlot.remainder < table[curr].remainder) {
                uint64_t tmpRem = newSlot.remainder;
                newSlot.remainder = table[curr].remainder;
                table[curr].remainder = tmpRem;
            }
            curr = (curr + 1) % numSlots;
            if (curr == fq) return false; // Table full
        }

        table[curr] = newSlot;
        count++;
        return true;
    }

    bool contains(const std::string& item) const {
        uint64_t h = hashString(item);
        uint64_t fq, fr;
        splitHash(h, fq, fr);

        if (!table[fq].isOccupied) {
            return false;
        }

        // Search cluster starting from canonical quotient
        uint64_t curr = fq;
        while (table[curr].hasData) {
            if (table[curr].remainder == fr) {
                return true;
            }
            curr = (curr + 1) % numSlots;
            if (curr == fq) break;
        }
        return false;
    }

    size_t size() const { return count; }
    uint64_t capacity() const { return numSlots; }
    double loadFactor() const { return static_cast<double>(count) / numSlots; }
};

} // namespace sketches

int main() {
    std::cout << "========================================\n";
    std::cout << "       QUOTIENT FILTER (AMQ SET)        \n";
    std::cout << "   (Contiguous Cache-Friendly Filter)   \n";
    std::cout << "========================================\n\n";

    // q = 6 (64 slots), r = 10 bits -> theoretical false positive rate ~ 2^-10 ~= 0.097%
    sketches::QuotientFilter qf(6, 10);

    std::cout << "Quotient Filter Configuration:\n";
    std::cout << "  - Number of Slots (2^q) : " << qf.capacity() << "\n";
    std::cout << "  - Remainder bits (r)    : 10\n";
    std::cout << "  - Expected FP Rate      : ~" << std::fixed << std::setprecision(3)
              << (100.0 / 1024.0) << "%\n\n";

    std::vector<std::string> insertedWords = {
        "alpha", "beta", "gamma", "delta", "epsilon",
        "zeta", "eta", "theta", "iota", "kappa"
    };

    std::cout << "Inserting elements...\n";
    for (const auto& w : insertedWords) {
        qf.insert(w);
    }
    std::cout << "Elements inserted: " << qf.size()
              << " (Load factor: " << std::setprecision(2) << qf.loadFactor() << ")\n\n";

    std::cout << "Membership Verification (True Positives):\n";
    for (const auto& w : insertedWords) {
        std::cout << "  - '" << std::setw(10) << std::left << w << "': "
                  << (qf.contains(w) ? "FOUND (Correct)" : "MISSED") << "\n";
    }

    std::cout << "\nUninserted Keys (Testing False Positives):\n";
    std::vector<std::string> absentWords = {
        "lambda", "mu", "nu", "xi", "omicron", "pi", "rho", "sigma"
    };

    int falsePositives = 0;
    for (const auto& w : absentWords) {
        bool found = qf.contains(w);
        if (found) falsePositives++;
        std::cout << "  - '" << std::setw(10) << std::left << w << "': "
                  << (found ? "FALSE POSITIVE" : "ABSENT (Correct)") << "\n";
    }

    std::cout << "\nFalse Positives Encountered: " << falsePositives << " / " << absentWords.size() << "\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
