/**
 * @file weighted_reservoir_sampling.cpp
 * @brief Efraimidis-Spirakis Weighted Random Reservoir Sampling (Algorithm A-Res) in C++17.
 *
 * Problem:
 *   Given a stream of items (x_i, w_i) with positive weights w_i > 0 and unknown total length N,
 *   select a random sample of K items without replacement such that items with higher
 *   weights are selected with correspondingly higher probability.
 *
 * Algorithm A-Res (Efraimidis & Spirakis, 2006):
 *   1. For each arriving item i with weight w_i > 0:
 *      Draw u_i ~ Uniform(0, 1).
 *      Compute key r_i = u_i^(1 / w_i)  (or log(r_i) = ln(u_i) / w_i for numerical stability).
 *   2. Maintain a min-heap (reservoir) of capacity K containing the K items with the LARGEST keys.
 *   3. When a new item has a key larger than the minimum key currently in the reservoir,
 *      replace the minimum item in the reservoir.
 *
 * Mathematical Property:
 *   For K = 1, the probability that item i is selected equals w_i / sum(w_j).
 *   For K > 1, items are chosen sequentially proportional to their weights without replacement.
 *
 * Complexity:
 *   - Time: O(N log K) for stream of N items.
 *   - Space: O(K) reservoir storage.
 */

#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <cmath>
#include <string>
#include <iomanip>
#include <unordered_map>

namespace sampling {

template <typename T>
struct WeightedItem {
    T item;
    double weight;
};

template <typename T>
class WeightedReservoir {
private:
    struct HeapEntry {
        double key; // ln(u) / w
        T item;

        // Min-heap comparator (smallest key at top)
        bool operator>(const HeapEntry& other) const {
            return key > other.key;
        }
    };

    size_t k;
    std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>> minHeap;
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> uniformDist;

public:
    explicit WeightedReservoir(size_t sampleSize, uint64_t seed = 42)
        : k(sampleSize), rng(seed), uniformDist(1e-15, 1.0 - 1e-15) {}

    void process(const T& item, double weight) {
        if (weight <= 0.0 || k == 0) return;

        double u = uniformDist(rng);
        double logKey = std::log(u) / weight; // Monotonic transformation of u^(1/w)

        if (minHeap.size() < k) {
            minHeap.push({logKey, item});
        } else if (logKey > minHeap.top().key) {
            minHeap.pop();
            minHeap.push({logKey, item});
        }
    }

    std::vector<T> getSample() const {
        auto copyHeap = minHeap;
        std::vector<T> result;
        result.reserve(copyHeap.size());
        while (!copyHeap.empty()) {
            result.push_back(copyHeap.top().item);
            copyHeap.pop();
        }
        return result;
    }

    void reset() {
        while (!minHeap.empty()) minHeap.pop();
    }
};

} // namespace sampling

int main() {
    std::cout << "========================================\n";
    std::cout << "  WEIGHTED RESERVOIR SAMPLING (A-RES)   \n";
    std::cout << "========================================\n\n";

    std::vector<sampling::WeightedItem<std::string>> dataset = {
        {"Server_A", 50.0}, // 50% weight
        {"Server_B", 30.0}, // 30% weight
        {"Server_C", 15.0}, // 15% weight
        {"Server_D",  5.0}  //  5% weight
    };

    double totalWeight = 100.0;

    // 1. Monte Carlo empirical validation for K = 1 over 20,000 trials
    const int TRIALS = 20000;
    std::unordered_map<std::string, int> winCounts;
    sampling::WeightedReservoir<std::string> singleReservoir(1, 2026);

    for (int t = 0; t < TRIALS; ++t) {
        singleReservoir.reset();
        for (const auto& entry : dataset) {
            singleReservoir.process(entry.item, entry.weight);
        }
        auto chosen = singleReservoir.getSample();
        if (!chosen.empty()) {
            winCounts[chosen[0]]++;
        }
    }

    std::cout << "Monte Carlo Validation (K = 1, " << TRIALS << " independent stream passes):\n";
    std::cout << std::left << std::setw(12) << "Item"
              << std::setw(12) << "Weight"
              << std::setw(18) << "Expected Prob"
              << "Observed Freq\n";
    std::cout << std::string(58, '-') << "\n";

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& entry : dataset) {
        double expected = (entry.weight / totalWeight) * 100.0;
        double observed = (static_cast<double>(winCounts[entry.item]) / TRIALS) * 100.0;

        std::cout << std::left << std::setw(12) << entry.item
                  << std::setw(12) << entry.weight
                  << std::setw(18) << (std::to_string(expected).substr(0, 5) + "%")
                  << observed << "%\n";
    }

    // 2. Sampling K = 2 items without replacement from a stream of 6 items
    std::cout << "\nMulti-item Stream Sampling (K = 2 from 6 weighted elements):\n";
    std::vector<sampling::WeightedItem<std::string>> stream = {
        {"Task_Alpha", 1.0}, {"Task_Beta", 20.0}, {"Task_Gamma", 2.0},
        {"Task_Delta", 40.0}, {"Task_Epsilon", 10.0}, {"Task_Zeta", 5.0}
    };

    sampling::WeightedReservoir<std::string> multiReservoir(2, 999);
    for (const auto& entry : stream) {
        multiReservoir.process(entry.item, entry.weight);
    }

    auto selected = multiReservoir.getSample();
    std::cout << "  Selected Reservoir Sample (Size 2): [ ";
    for (size_t i = 0; i < selected.size(); ++i) {
        std::cout << selected[i] << (i + 1 == selected.size() ? "" : ", ");
    }
    std::cout << " ]\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
