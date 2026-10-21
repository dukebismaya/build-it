/*
 * Program Description:
 * Fractional Knapsack Problem using Greedy Strategy in C++.
 * Given weights and values of N items, and a knapsack of capacity W,
 * find the maximum total value possible where items can be broken into fractions.
 *
 * Greedy Choice Property:
 * - Calculate value-per-unit-weight ratio (value / weight) for each item.
 * - Sort items in descending order of ratio.
 * - Greedily take as much of the item with the highest ratio as possible.
 * - If the knapsack cannot accommodate the full item, take the remaining fraction.
 *
 * Time Complexity: O(N log N) due to sorting.
 * Space Complexity: O(N).
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

struct Item {
    int id;
    double weight;
    double value;
    double ratio;

    Item(int i, double w, double v) : id(i), weight(w), value(v), ratio(v / w) {}
};

struct KnapsackResult {
    double totalValue;
    std::vector<std::pair<int, double>> itemFractions; // item id, fraction taken (0.0 to 1.0)
};

KnapsackResult fractionalKnapsack(double capacity, std::vector<Item> items) {
    // Sort items by value-to-weight ratio descending
    std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) {
        return a.ratio > b.ratio;
    });

    double currentWeight = 0.0;
    double totalValue = 0.0;
    std::vector<std::pair<int, double>> fractions;

    for (const auto& item : items) {
        if (currentWeight + item.weight <= capacity) {
            // Take full item
            currentWeight += item.weight;
            totalValue += item.value;
            fractions.push_back({item.id, 1.0});
        } else {
            // Take fraction of item
            double remainingCap = capacity - currentWeight;
            if (remainingCap > 0) {
                double fraction = remainingCap / item.weight;
                totalValue += item.value * fraction;
                currentWeight += remainingCap;
                fractions.push_back({item.id, fraction});
            }
            break; // Knapsack is full
        }
    }

    return {totalValue, fractions};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Fractional Knapsack (Greedy)        \n";
    std::cout << "==========================================\n\n";

    double capacity = 50.0;
    std::vector<Item> items = {
        Item(1, 10.0, 60.0),   // Ratio: 6.0
        Item(2, 20.0, 100.0),  // Ratio: 5.0
        Item(3, 30.0, 120.0)   // Ratio: 4.0
    };

    std::cout << "Knapsack Capacity: " << capacity << " kg\n\n";
    std::cout << "Available Items:\n";
    std::cout << "  ID | Weight (kg) | Value ($) | Ratio ($/kg)\n";
    std::cout << "  ---+-------------+-----------+-------------\n";
    for (const auto& it : items) {
        std::cout << "  " << std::setw(2) << it.id << " | "
                  << std::setw(11) << it.weight << " | "
                  << std::setw(9) << it.value << " | "
                  << std::setw(12) << std::fixed << std::setprecision(2) << it.ratio << "\n";
    }

    auto result = fractionalKnapsack(capacity, items);

    std::cout << "\n--- Optimal Selection ---\n";
    for (const auto& [id, frac] : result.itemFractions) {
        std::cout << "  Item #" << id << ": " << std::setprecision(1) << (frac * 100.0) << "% taken\n";
    }

    std::cout << "\nTotal Maximized Value: $" << std::setprecision(2) << result.totalValue
              << " (Expected: $240.00)\n";

    return 0;
}
