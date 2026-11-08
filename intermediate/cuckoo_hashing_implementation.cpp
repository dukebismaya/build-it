/**
 * @file cuckoo_hashing_implementation.cpp
 * @brief Cuckoo Hashing table with O(1) worst-case lookup and deletion in C++17.
 *
 * Cuckoo Hashing (Pagh and Rodler, 2001) is a hashing scheme that guarantees:
 *   - O(1) worst-case lookup time.
 *   - O(1) worst-case deletion time.
 *   - O(1) expected insertion time.
 *
 * Architecture:
 *   - Two hash tables: T1 and T2, each of size M.
 *   - Two independent universal hash functions: h1(key) and h2(key).
 *   - A key x can ONLY reside in T1[h1(x)] or T2[h2(x)].
 *
 * Insertion Mechanism (The "Cuckoo" kick-out):
 *   1. Attempt to place x into T1[h1(x)].
 *   2. If the slot is empty, store x.
 *   3. If occupied by y, kick y out and place x.
 *   4. Attempt to place y into its alternate location in T2[h2(y)].
 *   5. If that slot is occupied by z, kick z out, and repeat.
 *   6. If an eviction cycle is detected (chain length exceeds MAX_LOOP), rehash
 *      with new random hash coefficients and larger capacity.
 *
 * Complexity:
 *   - Lookup: Exactly at most 2 table probes -> O(1) worst-case.
 *   - Deletion: At most 2 table probes -> O(1) worst-case.
 *   - Insertion: O(1) amortized/expected time.
 */

#include <iostream>
#include <vector>
#include <optional>
#include <random>
#include <string>
#include <iomanip>

namespace hashing {

template <typename Key, typename Value>
class CuckooHashTable {
private:
    struct Entry {
        Key key;
        Value value;
    };

    size_t capacity;
    std::vector<std::optional<Entry>> table1;
    std::vector<std::optional<Entry>> table2;
    size_t numElements;

    // Hash parameters for ax + b mod P
    uint64_t a1, b1;
    uint64_t a2, b2;
    static constexpr uint64_t PRIME = 1000000007ULL;

    std::mt19937_64 rng;

    void generateHashFunctions() {
        std::uniform_int_distribution<uint64_t> distA(1, PRIME - 1);
        std::uniform_int_distribution<uint64_t> distB(0, PRIME - 1);
        a1 = distA(rng);
        b1 = distB(rng);
        a2 = distA(rng);
        b2 = distB(rng);
    }

    size_t hash1(const Key& k) const {
        std::hash<Key> hasher;
        uint64_t h = hasher(k);
        return ((a1 * h + b1) % PRIME) % capacity;
    }

    size_t hash2(const Key& k) const {
        std::hash<Key> hasher;
        uint64_t h = hasher(k);
        return ((a2 * h + b2) % PRIME) % capacity;
    }

    void rehash(size_t newCapacity) {
        std::vector<Entry> existingEntries;
        existingEntries.reserve(numElements);

        for (const auto& opt : table1) {
            if (opt) existingEntries.push_back(*opt);
        }
        for (const auto& opt : table2) {
            if (opt) existingEntries.push_back(*opt);
        }

        capacity = newCapacity;
        table1.assign(capacity, std::nullopt);
        table2.assign(capacity, std::nullopt);
        numElements = 0;
        generateHashFunctions();

        for (const auto& entry : existingEntries) {
            insert(entry.key, entry.value);
        }
    }

public:
    explicit CuckooHashTable(size_t initialCapacity = 16)
        : capacity(initialCapacity), table1(initialCapacity, std::nullopt),
          table2(initialCapacity, std::nullopt), numElements(0), rng(1337) {
        generateHashFunctions();
    }

    std::optional<Value> find(const Key& key) const {
        size_t idx1 = hash1(key);
        if (table1[idx1] && table1[idx1]->key == key) {
            return table1[idx1]->value;
        }

        size_t idx2 = hash2(key);
        if (table2[idx2] && table2[idx2]->key == key) {
            return table2[idx2]->value;
        }

        return std::nullopt;
    }

    bool remove(const Key& key) {
        size_t idx1 = hash1(key);
        if (table1[idx1] && table1[idx1]->key == key) {
            table1[idx1] = std::nullopt;
            numElements--;
            return true;
        }

        size_t idx2 = hash2(key);
        if (table2[idx2] && table2[idx2]->key == key) {
            table2[idx2] = std::nullopt;
            numElements--;
            return true;
        }

        return false;
    }

    void insert(Key key, Value value) {
        // If key already present, update in-place
        size_t idx1 = hash1(key);
        if (table1[idx1] && table1[idx1]->key == key) {
            table1[idx1]->value = value;
            return;
        }
        size_t idx2 = hash2(key);
        if (table2[idx2] && table2[idx2]->key == key) {
            table2[idx2]->value = value;
            return;
        }

        Entry currentEntry = {key, value};
        size_t maxLoop = 2 * capacity + 10;

        for (size_t loop = 0; loop < maxLoop; ++loop) {
            // Try inserting into Table 1
            size_t pos1 = hash1(currentEntry.key);
            if (!table1[pos1]) {
                table1[pos1] = currentEntry;
                numElements++;
                return;
            }
            // Displace occupant in Table 1
            std::swap(currentEntry, *table1[pos1]);

            // Try inserting displaced item into Table 2
            size_t pos2 = hash2(currentEntry.key);
            if (!table2[pos2]) {
                table2[pos2] = currentEntry;
                numElements++;
                return;
            }
            // Displace occupant in Table 2
            std::swap(currentEntry, *table2[pos2]);
        }

        // Cycle detected: rehash with double capacity
        rehash(capacity * 2);
        insert(currentEntry.key, currentEntry.value);
    }

    size_t size() const { return numElements; }
    size_t getCapacity() const { return capacity; }
};

} // namespace hashing

int main() {
    std::cout << "========================================\n";
    std::cout << "     CUCKOO HASH TABLE IMPLEMENTATION   \n";
    std::cout << "========================================\n\n";

    hashing::CuckooHashTable<std::string, int> phoneBook(8);

    std::vector<std::pair<std::string, int>> entries = {
        {"Alice", 101}, {"Bob", 202}, {"Charlie", 303}, {"David", 404},
        {"Emma", 505}, {"Frank", 606}, {"Grace", 707}, {"Henry", 808},
        {"Ivy", 909}, {"Jack", 1001}
    };

    std::cout << "Inserting " << entries.size() << " key-value pairs...\n";
    for (const auto& item : entries) {
        phoneBook.insert(item.first, item.second);
        std::cout << "  Inserted: " << std::left << std::setw(10) << item.first
                  << " -> " << item.second
                  << " (Table size: " << phoneBook.size()
                  << ", Capacity: " << phoneBook.getCapacity() << ")\n";
    }

    std::cout << "\nWorst-Case O(1) Lookups:\n";
    std::vector<std::string> lookupQueries = {"Alice", "David", "Grace", "Zoe"};
    for (const auto& query : lookupQueries) {
        auto val = phoneBook.find(query);
        std::cout << "  Lookup '" << std::left << std::setw(8) << query << "': ";
        if (val) {
            std::cout << "Found (Value: " << *val << ")\n";
        } else {
            std::cout << "NOT FOUND\n";
        }
    }

    std::cout << "\nO(1) Deletion Demonstration:\n";
    std::cout << "  Deleting 'Bob'...\n";
    bool removedBob = phoneBook.remove("Bob");
    std::cout << "  Removed? " << (removedBob ? "YES" : "NO") << "\n";
    std::cout << "  Lookup 'Bob' after deletion: "
              << (phoneBook.find("Bob") ? "Found" : "NOT FOUND") << "\n";
    std::cout << "  Current table size: " << phoneBook.size() << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
