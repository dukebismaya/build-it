/*
 * Program Description:
 * Tower of Hanoi Puzzle Solver in C++.
 * Mathematical puzzle with three rods (Source, Auxiliary, Target) and N disks.
 * Rules:
 * 1. Only one disk can be moved at a time.
 * 2. Each move consists of taking the upper disk from one of the stacks and placing it on another.
 * 3. No larger disk may be placed on top of a smaller disk.
 *
 * Minimum moves required for N disks: 2^N - 1.
 * Time Complexity: O(2^N)
 * Space Complexity: O(N) for recursion stack.
 */

#include <iostream>
#include <vector>
#include <cmath>

class TowerOfHanoi {
    long long moveCount = 0;

public:
    void solve(int n, char fromRod, char toRod, char auxRod) {
        if (n == 0) return;

        // Move top n-1 disks from source to auxiliary rod
        solve(n - 1, fromRod, auxRod, toRod);

        // Move the nth disk from source to target rod
        moveCount++;
        std::cout << "Step " << moveCount << ": Move disk " << n
                  << " from peg " << fromRod << " -> peg " << toRod << "\n";

        // Move the n-1 disks from auxiliary to target rod
        solve(n - 1, auxRod, toRod, fromRod);
    }

    long long getMoveCount() const {
        return moveCount;
    }

    void reset() {
        moveCount = 0;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "          Tower of Hanoi Solver           \n";
    std::cout << "==========================================\n\n";

    int disks = 3;
    TowerOfHanoi hanoi;

    std::cout << "Solving Tower of Hanoi for N = " << disks << " disks (Pegs A, B, C):\n\n";
    hanoi.solve(disks, 'A', 'C', 'B');

    long long theoreticalMin = (1LL << disks) - 1;
    std::cout << "\nTotal Moves Performed: " << hanoi.getMoveCount() << "\n";
    std::cout << "Theoretical Minimum (2^" << disks << " - 1): " << theoreticalMin << "\n";
    std::cout << "Verification: " << (hanoi.getMoveCount() == theoreticalMin ? "MATCH [PASS]" : "FAIL") << "\n\n";

    std::cout << "--- Minimum moves reference table ---\n";
    for (int d = 1; d <= 10; ++d) {
        std::cout << "Disks = " << d << " -> " << ((1LL << d) - 1) << " moves\n";
    }

    return 0;
}
