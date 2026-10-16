/*
 * Program Description:
 * Fisher-Yates (Knuth) In-Place Shuffle Algorithm.
 * Generates an unbiased, uniformly distributed random permutation of an array in O(N) time.
 *
 * Algorithm Details:
 * To shuffle an array a of n elements:
 * For i from n - 1 down to 1:
 *   Pick a random integer j such that 0 <= j <= i
 *   Swap a[i] and a[j]
 *
 * This implementation includes:
 * 1. Generic template-based shuffle function using C++11 <random> (std::mt19937).
 * 2. Monte Carlo statistical test demonstrating uniform 1/N! distribution across all permutations.
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include <string>
#include <iomanip>

template <typename T>
void fisherYatesShuffle(std::vector<T>& arr, std::mt19937& rng) {
    int n = static_cast<int>(arr.size());
    for (int i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(rng);
        std::swap(arr[i], arr[j]);
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Fisher-Yates Shuffle Demo         \n";
    std::cout << "==========================================\n\n";

    std::random_device rd;
    std::mt19937 rng(rd());

    std::vector<int> cards = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::cout << "Original Array: ";
    for (int x : cards) std::cout << x << " ";
    std::cout << "\n\n";

    std::cout << "--- 5 Successive Random Shuffles ---\n";
    for (int trial = 1; trial <= 5; ++trial) {
        fisherYatesShuffle(cards, rng);
        std::cout << "Shuffle #" << trial << ": ";
        for (int x : cards) std::cout << x << " ";
        std::cout << "\n";
    }

    std::cout << "\n--- Statistical Uniformity Test (60,000 Trials on [A, B, C]) ---\n";
    std::map<std::string, int> permCounts;
    const int totalTrials = 60000;

    for (int t = 0; t < totalTrials; ++t) {
        std::vector<char> sample = {'A', 'B', 'C'};
        fisherYatesShuffle(sample, rng);
        std::string perm(sample.begin(), sample.end());
        permCounts[perm]++;
    }

    std::cout << "Expected per permutation: " << (totalTrials / 6) << " (16.67%)\n\n";
    std::cout << std::fixed << std::setprecision(2);
    for (const auto& [perm, count] : permCounts) {
        double pct = (static_cast<double>(count) / totalTrials) * 100.0;
        std::cout << "Permutation [" << perm << "]: "
                  << std::setw(6) << count << " occurrences (" << pct << "%)\n";
    }

    return 0;
}
