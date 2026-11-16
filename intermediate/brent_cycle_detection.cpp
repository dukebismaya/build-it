/**
 * ============================================================================
 * Brent's Cycle Detection Algorithm ("Teleporting Turtle" Algorithm)
 * ============================================================================
 * 
 * Description:
 * Brent's cycle detection algorithm (developed by Richard P. Brent in 1980)
 * is an improvement over Floyd's tortoise-and-hare algorithm for finding cycles
 * in iterated function sequences x_{i+1} = f(x_i).
 * 
 * Mechanism:
 * - Floyd's algorithm moves the tortoise by 1 step and the hare by 2 steps.
 * - Brent's algorithm leaves the tortoise stationary at powers-of-two steps
 *   (1, 2, 4, 8, 16, ...) and only moves the hare forward one step at a time.
 * - The tortoise "teleports" to the hare's position when the step count reaches
 *   the next power of two.
 * - When hare == tortoise, a cycle has been detected! The number of steps taken
 *   since the last teleportation directly gives the cycle length lambda.
 * 
 * Finding Cycle Start (mu):
 * - To find the index mu where the cycle begins:
 *   1) Reset tortoise and hare to x_0.
 *   2) Advance the hare by lambda steps.
 *   3) Advance both tortoise and hare 1 step at a time until they meet.
 *   4) The position where they meet is the exact cycle start mu.
 * 
 * Performance Comparison with Floyd's Algorithm:
 * - Floyd's algorithm takes:
 *     Steps to meet: mu + lambda - (mu mod lambda)
 *     Function evaluations: 3 * (mu + lambda - (mu mod lambda))
 * - Brent's algorithm takes:
 *     At most 2 * max(mu, lambda) steps to detect.
 *     Cycle length lambda is found directly without extra steps.
 *     On average 24% to 36% fewer function evaluations than Floyd's!
 * 
 * Time Complexity:
 * - O(mu + lambda) function evaluations
 * - Space Complexity: O(1) auxiliary memory
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <functional>

namespace brent_cycle {

struct CycleResult {
    int64_t lambda; // Length of cycle
    int64_t mu;     // Start index of cycle (0-based)
    int64_t function_calls;
};

/**
 * Executes Brent's cycle detection algorithm on an abstract function f(x).
 */
CycleResult detectCycle(int64_t x0, const std::function<int64_t(int64_t)>& f) {
    int64_t power = 1;
    int64_t lambda = 1;
    int64_t tortoise = x0;
    int64_t hare = f(x0);
    int64_t fn_calls = 1;

    // Phase 1: Detect cycle and determine length lambda
    while (tortoise != hare) {
        if (power == lambda) {
            tortoise = hare;
            power *= 2;
            lambda = 0;
        }
        hare = f(hare);
        fn_calls++;
        lambda++;
    }

    // Phase 2: Find start index mu
    tortoise = x0;
    hare = x0;
    for (int64_t i = 0; i < lambda; ++i) {
        hare = f(hare);
        fn_calls++;
    }

    int64_t mu = 0;
    while (tortoise != hare) {
        tortoise = f(tortoise);
        hare = f(hare);
        fn_calls += 2;
        mu++;
    }

    return {lambda, mu, fn_calls};
}

/**
 * Standard Floyd's algorithm for benchmark comparison.
 */
CycleResult detectCycleFloyd(int64_t x0, const std::function<int64_t(int64_t)>& f) {
    int64_t tortoise = f(x0);
    int64_t hare = f(f(x0));
    int64_t fn_calls = 3;

    while (tortoise != hare) {
        tortoise = f(tortoise);
        hare = f(f(hare));
        fn_calls += 3;
    }

    // Find mu
    int64_t mu = 0;
    tortoise = x0;
    while (tortoise != hare) {
        tortoise = f(tortoise);
        hare = f(hare);
        fn_calls += 2;
        mu++;
    }

    // Find lambda
    int64_t lambda = 1;
    hare = f(tortoise);
    fn_calls++;
    while (tortoise != hare) {
        hare = f(hare);
        fn_calls++;
        lambda++;
    }

    return {lambda, mu, fn_calls};
}

} // namespace brent_cycle

int main() {
    std::cout << "========================================================\n";
    std::cout << " Brent's Cycle Detection Algorithm (Teleporting Turtle)\n";
    std::cout << "========================================================\n\n";

    // Test Case: Pseudorandom generator f(x) = (x^2 + 1) mod M (Pollard's rho generator)
    constexpr int64_t MOD = 10007;
    auto f = [](int64_t x) {
        return (x * x + 1) % MOD;
    };

    int64_t initial_seeds[] = {2, 7, 42, 105, 512, 1337};

    std::cout << "Comparison between Brent's and Floyd's Algorithm:\n";
    std::cout << "Function: f(x) = (x^2 + 1) mod " << MOD << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::setw(8) << "Seed"
              << std::setw(10) << "Start(mu)"
              << std::setw(12) << "Length(lam)"
              << std::setw(15) << "Brent Calls"
              << std::setw(15) << "Floyd Calls"
              << std::setw(10) << "Speedup\n";
    std::cout << "----------------------------------------------------------------------\n";

    for (int64_t seed : initial_seeds) {
        auto brent_res = brent_cycle::detectCycle(seed, f);
        auto floyd_res = brent_cycle::detectCycleFloyd(seed, f);

        double speedup = static_cast<double>(floyd_res.function_calls) / brent_res.function_calls;

        std::cout << std::setw(8) << seed
                  << std::setw(10) << brent_res.mu
                  << std::setw(12) << brent_res.lambda
                  << std::setw(15) << brent_res.function_calls
                  << std::setw(15) << floyd_res.function_calls
                  << std::setw(9) << std::fixed << std::setprecision(2) << speedup << "x\n";
    }

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Notice: Brent's algorithm consistently requires significantly fewer calls!\n";
    return 0;
}
