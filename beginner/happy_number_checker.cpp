/*
 * Program Description:
 * Happy Number Checker in C++.
 * A happy number is a number defined by the following process:
 * Starting with any positive integer, replace the number by the sum of the squares of its digits.
 * Repeat the process until the number equals 1 (where it will stay),
 * or it loops endlessly in a cycle which does not include 1 (unhappy).
 *
 * Approaches:
 * 1. Hash Set: Tracks visited numbers in O(k) space.
 * 2. Floyd's Cycle-Finding Algorithm (Tortoise and Hare):
 *    Slow pointer advances 1 step, fast pointer advances 2 steps.
 *    Detects cycle in O(1) auxiliary space!
 */

#include <iostream>
#include <vector>
#include <unordered_set>

// Computes the sum of squares of the digits of n
long long sumOfDigitSquares(long long n) {
    long long sum = 0;
    while (n > 0) {
        long long digit = n % 10;
        sum += digit * digit;
        n /= 10;
    }
    return sum;
}

// Approach 1: Floyd's Tortoise and Hare algorithm (O(1) extra space)
bool isHappyFloyd(long long n) {
    long long slow = n;
    long long fast = sumOfDigitSquares(n);

    while (fast != 1 && slow != fast) {
        slow = sumOfDigitSquares(slow);
        fast = sumOfDigitSquares(sumOfDigitSquares(fast));
    }

    return fast == 1;
}

// Traces and prints the transformation sequence of a number
void traceHappySequence(long long n) {
    std::cout << "Sequence for " << n << ": " << n;
    std::unordered_set<long long> visited;

    long long curr = n;
    while (curr != 1 && visited.find(curr) == visited.end()) {
        visited.insert(curr);
        curr = sumOfDigitSquares(curr);
        std::cout << " -> " << curr;
    }

    if (curr == 1) {
        std::cout << " [HAPPY NUMBER!]\n";
    } else {
        std::cout << " [CYCLE DETECTED: UNHAPPY NUMBER]\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "           Happy Number Checker           \n";
    std::cout << "==========================================\n\n";

    std::vector<long long> testCases = {19, 7, 2, 4, 100, 2026};

    for (long long num : testCases) {
        traceHappySequence(num);
        bool happy = isHappyFloyd(num);
        std::cout << "  Floyd O(1) Verification: " << (happy ? "HAPPY" : "UNHAPPY") << "\n\n";
    }

    std::cout << "First 10 Happy Numbers: ";
    int count = 0;
    for (int i = 1; count < 10; ++i) {
        if (isHappyFloyd(i)) {
            std::cout << i << " ";
            count++;
        }
    }
    std::cout << "\n";

    return 0;
}
