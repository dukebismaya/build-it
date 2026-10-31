/**
 * @file simulated_annealing_tsp.cpp
 * @brief Solves the Travelling Salesperson Problem (TSP) using Simulated Annealing with 2-opt moves.
 *
 * Concepts:
 * - TSP is an NP-hard combinatorial optimization problem.
 * - Simulated Annealing (Kirkpatrick, Gelatt, Vecchi 1983) is a metaheuristic modeled
 *   after the physical annealing process in metallurgy.
 * - Acceptance Criterion (Metropolis-Hastings):
 *     If newCost < currentCost: always accept.
 *     Else: accept with probability P = exp(-(newCost - currentCost) / Temperature).
 * - Neighborhood Generation:
 *     2-opt inversion move: reverses the subsequence between two randomly chosen indices.
 * - Temperature Schedule:
 *     Geometric cooling: T_{k+1} = alpha * T_k  (e.g., alpha = 0.995).
 *
 * Time Complexity: O(Iterations * 1) per step (using 2-opt delta evaluation)
 * Space Complexity: O(N) for the city tour.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <iomanip>

struct Point2D {
    double x;
    double y;
};

double euclideanDistance(const Point2D& a, const Point2D& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

class SimulatedAnnealingTSP {
private:
    std::vector<Point2D> cities;
    int n;
    std::mt19937 rng;

public:
    explicit SimulatedAnnealingTSP(std::vector<Point2D> cityCoords, unsigned int seed = 42)
        : cities(std::move(cityCoords)), n(static_cast<int>(cities.size())), rng(seed) {}

    double computeTourLength(const std::vector<int>& tour) const {
        double total = 0.0;
        for (int i = 0; i < n; ++i) {
            int next = (i + 1) % n;
            total += euclideanDistance(cities[tour[i]], cities[tour[next]]);
        }
        return total;
    }

    struct AnnealingResult {
        std::vector<int> bestTour;
        double initialCost;
        double bestCost;
        int iterations;
        int acceptedMoves;
        int worseningAcceptedMoves;
    };

    /**
     * @brief Executes Simulated Annealing with geometric cooling.
     * @param initialTemp Starting temperature.
     * @param finalTemp Minimum temperature to stop annealing.
     * @param coolingRate Alpha parameter in T = alpha * T.
     * @param stepsPerTemp Number of 2-opt proposals per temperature step.
     */
    AnnealingResult solve(double initialTemp = 1000.0,
                          double finalTemp = 1e-4,
                          double coolingRate = 0.995,
                          int stepsPerTemp = 100) {
        // Initial tour: 0, 1, 2, ..., n-1
        std::vector<int> currentTour(n);
        std::iota(currentTour.begin(), currentTour.end(), 0);
        std::shuffle(currentTour.begin() + 1, currentTour.end(), rng);

        double currentCost = computeTourLength(currentTour);
        double initialCost = currentCost;

        std::vector<int> bestTour = currentTour;
        double bestCost = currentCost;

        double T = initialTemp;
        std::uniform_real_distribution<double> uniform01(0.0, 1.0);
        std::uniform_int_distribution<int> cityDist(0, n - 1);

        int totalIterations = 0;
        int totalAccepted = 0;
        int worseningAccepted = 0;

        while (T > finalTemp) {
            for (int step = 0; step < stepsPerTemp; ++step) {
                totalIterations++;

                // Select two distinct cut points i < j for 2-opt
                int i = cityDist(rng);
                int j = cityDist(rng);
                if (i > j) std::swap(i, j);
                if (i == j || (i == 0 && j == n - 1)) continue;

                // Compute delta cost for reversing tour[i..j]
                int prevI = (i - 1 + n) % n;
                int nextJ = (j + 1) % n;

                int u = currentTour[prevI];
                int v = currentTour[i];
                int w = currentTour[j];
                int z = currentTour[nextJ];

                double oldEdges = euclideanDistance(cities[u], cities[v]) +
                                  euclideanDistance(cities[w], cities[z]);
                double newEdges = euclideanDistance(cities[u], cities[w]) +
                                  euclideanDistance(cities[v], cities[z]);
                double delta = newEdges - oldEdges;

                bool accept = false;
                if (delta < 0.0) {
                    accept = true;
                } else {
                    double prob = std::exp(-delta / T);
                    if (uniform01(rng) < prob) {
                        accept = true;
                        worseningAccepted++;
                    }
                }

                if (accept) {
                    totalAccepted++;
                    std::reverse(currentTour.begin() + i, currentTour.begin() + j + 1);
                    currentCost += delta;

                    if (currentCost < bestCost) {
                        bestCost = currentCost;
                        bestTour = currentTour;
                    }
                }
            }

            T *= coolingRate;
        }

        return {bestTour, initialCost, bestCost, totalIterations, totalAccepted, worseningAccepted};
    }
};

int main() {
    std::cout << "=== Simulated Annealing for Travelling Salesperson Problem (TSP) ===\n\n";

    // 10 Cities placed roughly on a circle of radius 100
    // Optimal tour traverses vertices in cyclic order with perimeter ~ 2 * pi * 100 ~ 618.0
    const int numCities = 12;
    std::vector<Point2D> circleCities(numCities);
    const double radius = 100.0;
    const double PI = 3.14159265358979323846;

    std::cout << "Generating " << numCities << " cities on a circle:\n";
    for (int i = 0; i < numCities; ++i) {
        double angle = 2.0 * PI * i / numCities;
        circleCities[i] = {radius * std::cos(angle), radius * std::sin(angle)};
        std::cout << "  City " << std::setw(2) << i << ": ("
                  << std::fixed << std::setprecision(2) << std::setw(7) << circleCities[i].x << ", "
                  << std::setw(7) << circleCities[i].y << ")\n";
    }

    SimulatedAnnealingTSP tsp(circleCities, 12345);
    auto res = tsp.solve(1000.0, 1e-4, 0.995, 200);

    double optimalCirclePerimeter = 2.0 * numCities * radius * std::sin(PI / numCities);

    std::cout << "\nOptimization Results:\n";
    std::cout << "  Initial Random Tour Length: " << std::fixed << std::setprecision(3) << res.initialCost << '\n';
    std::cout << "  Annealed Best Tour Length:  " << res.bestCost << '\n';
    std::cout << "  Known Geometric Optimum:    " << optimalCirclePerimeter << '\n';
    std::cout << "  Optimality Gap:             " 
              << ((res.bestCost - optimalCirclePerimeter) / optimalCirclePerimeter * 100.0) << " %\n\n";

    std::cout << "Statistics:\n";
    std::cout << "  Total 2-Opt Iterations:     " << res.iterations << '\n';
    std::cout << "  Accepted Transitions:       " << res.acceptedMoves << '\n';
    std::cout << "  Uphill (Worsening) Accepts: " << res.worseningAcceptedMoves << '\n';

    std::cout << "\nBest Tour Path: ";
    for (int city : res.bestTour) {
        std::cout << city << " -> ";
    }
    std::cout << res.bestTour[0] << '\n';

    return 0;
}
