/**
 * @file reservoir_sampling_algorithm.cpp
 * @brief Implements Reservoir Sampling (Algorithm R) for uniformly sampling k items from a data stream.
 *
 * Problem:
 * Choose k items uniformly at random from a stream of items of unknown, arbitrarily large,
 * or infinite length N in a single pass, using O(k) memory.
 *
 * Algorithm (Waterman / Vitter):
 * 1. Initialize the reservoir with the first k elements of the stream.
 * 2. For each subsequent element stream[i] (where i = k, k+1, ..., N-1):
 *    - Generate a random integer j in the range [0, i].
 *    - If j < k, replace reservoir[j] with stream[i].
 *
 * Mathematical Invariant:
 * After processing i+1 elements, every element seen so far has an equal probability
 * k / (i + 1) of being in the reservoir.
 *
 * Time Complexity: O(N) single pass
 * Space Complexity: O(k)
 */

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <cmath>

class ReservoirSampler {
private:
    std::mt19937 rng;

public:
    explicit ReservoirSampler(unsigned int seed = 42) : rng(seed) {}

    /**
     * @brief Samples k elements uniformly from a stream.
     * @param stream Input stream of data.
     * @param k Desired sample size.
     * @return Vector of k randomly sampled elements.
     */
    std::vector<int> sample(const std::vector<int>& stream, int k) {
        int n = static_cast<int>(stream.size());
        if (k <= 0) return {};
        if (n <= k) return stream;

        // 1. Fill reservoir with first k elements
        std::vector<int> reservoir(stream.begin(), stream.begin() + k);

        // 2. Process remaining elements
        for (int i = k; i < n; ++i) {
            std::uniform_int_distribution<int> dist(0, i);
            int j = dist(rng);

            if (j < k) {
                reservoir[j] = stream[i];
            }
        }

        return reservoir;
    }
};

int main() {
    std::cout << "=== Reservoir Sampling (Algorithm R) ===\n\n";

    // 1. Single Sampling Demo
    const int N = 20;
    const int K = 5;
    std::vector<int> stream(N);
    for (int i = 0; i < N; ++i) stream[i] = i * 10; // 0, 10, 20, ..., 190

    std::cout << "Stream of " << N << " items: [ ";
    for (int v : stream) std::cout << v << " ";
    std::cout << "]\n";

    ReservoirSampler sampler(1337);
    auto sampleResult = sampler.sample(stream, K);

    std::cout << "Uniform Random Sample of size k = " << K << ": [ ";
    for (int v : sampleResult) std::cout << v << " ";
    std::cout << "]\n\n";

    // 2. Empirical Uniformity Verification across 100,000 trials
    const int TRIALS = 100000;
    const int STREAM_SIZE = 10;
    const int SAMPLE_SIZE = 3;
    std::vector<int> smallStream = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> frequency(STREAM_SIZE, 0);

    for (int t = 0; t < TRIALS; ++t) {
        auto s = sampler.sample(smallStream, SAMPLE_SIZE);
        for (int item : s) {
            frequency[item]++;
        }
    }

    double expectedProbability = static_cast<double>(SAMPLE_SIZE) / STREAM_SIZE;
    double expectedCount = TRIALS * expectedProbability;

    std::cout << "2. Uniform Distribution Test across " << TRIALS << " Trials (k = "
              << SAMPLE_SIZE << ", N = " << STREAM_SIZE << "):\n";
    std::cout << "  Expected Selection Probability: " << (expectedProbability * 100.0) << " %\n";
    std::cout << "  Expected Count per Element:     " << std::fixed << std::setprecision(1) << expectedCount << "\n\n";

    std::cout << "  " << std::setw(8) << "Element"
              << std::setw(16) << "Observed Count"
              << std::setw(16) << "Empirical %"
              << std::setw(16) << "Error %" << '\n';
    std::cout << "  " << std::string(56, '-') << '\n';

    for (int i = 0; i < STREAM_SIZE; ++i) {
        double empiricalPct = (static_cast<double>(frequency[i]) / TRIALS) * 100.0;
        double errorPct = std::abs(frequency[i] - expectedCount) / expectedCount * 100.0;

        std::cout << "  " << std::setw(8) << i
                  << std::setw(16) << frequency[i]
                  << std::setw(15) << std::fixed << std::setprecision(2) << empiricalPct << " %"
                  << std::setw(15) << std::fixed << std::setprecision(2) << errorPct << " %\n";
    }

    return 0;
}
