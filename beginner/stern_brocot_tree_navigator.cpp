/*
 * ============================================================================
 *  Stern-Brocot Tree Navigator & Rational Approximation
 * ============================================================================
 *
 *  The Stern-Brocot tree is an infinite binary search tree containing every
 *  positive rational number exactly once in its lowest terms. It is constructed
 *  by mediant insertion between neighboring fractions.
 *
 *  Starting with sentinels 0/1 and 1/0:
 *    - The root is the mediant: (0+1)/(1+0) = 1/1
 *    - Left child of p/q between a/b and p/q: mediant of a/b and p/q
 *    - Right child of p/q between p/q and c/d: mediant of p/q and c/d
 *
 *  Applications:
 *  - Finding the best rational approximation of a real number with
 *    bounded denominator (like continued fraction convergents).
 *  - Encoding any positive rational as a binary path (L/R string).
 *  - Clock arithmetic, Farey sequences, and number theory.
 *
 *  This program:
 *  1) Navigates the tree to find the path (L/R) to a given fraction.
 *  2) Finds the best rational approximation of a decimal with max denominator.
 *  3) Enumerates fractions at a given depth level.
 *
 *  Time Complexity:  O(a + b) to locate fraction a/b in the tree
 *  Space Complexity: O(1) auxiliary for navigation
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <cmath>
#include <iomanip>

namespace stern_brocot {

struct Fraction {
    int64_t num, den;
    double value() const { return static_cast<double>(num) / den; }
};

/*
 * Find the path from the root to fraction p/q in the Stern-Brocot tree.
 * Returns a string of 'L' and 'R' characters.
 * Assumes p/q is in lowest terms and positive.
 */
std::string findPath(int64_t p, int64_t q) {
    std::string path;
    int64_t lNum = 0, lDen = 1; /* Left sentinel 0/1 */
    int64_t rNum = 1, rDen = 0; /* Right sentinel 1/0 */

    while (true) {
        int64_t mNum = lNum + rNum;
        int64_t mDen = lDen + rDen;

        if (mNum == p && mDen == q) {
            break; /* Found it */
        } else if (p * mDen < mNum * q) {
            /* Target is left of mediant */
            path += 'L';
            rNum = mNum;
            rDen = mDen;
        } else {
            /* Target is right of mediant */
            path += 'R';
            lNum = mNum;
            lDen = mDen;
        }
    }

    return path;
}

/*
 * Best rational approximation of target with denominator <= maxDen.
 * Walks the Stern-Brocot tree, choosing L/R based on comparison with target.
 */
Fraction bestApproximation(double target, int64_t maxDen) {
    int64_t lNum = 0, lDen = 1;
    int64_t rNum = 1, rDen = 0;
    Fraction best = {0, 1};
    double bestErr = std::abs(target - best.value());

    while (true) {
        int64_t mNum = lNum + rNum;
        int64_t mDen = lDen + rDen;

        if (mDen > maxDen) break;

        double mVal = static_cast<double>(mNum) / mDen;
        double err = std::abs(target - mVal);

        if (err < bestErr) {
            bestErr = err;
            best = {mNum, mDen};
        }

        if (std::abs(mVal - target) < 1e-15) {
            break;
        } else if (target < mVal) {
            rNum = mNum;
            rDen = mDen;
        } else {
            lNum = mNum;
            lDen = mDen;
        }
    }

    return best;
}

/*
 * Enumerate all fractions at a given depth in the Stern-Brocot tree.
 * Depth 0 = root (1/1), depth 1 = {1/2, 2/1}, etc.
 */
void enumerateAtDepth(int64_t lNum, int64_t lDen, int64_t rNum, int64_t rDen,
                      int currentDepth, int targetDepth,
                      std::vector<Fraction>& result) {
    int64_t mNum = lNum + rNum;
    int64_t mDen = lDen + rDen;

    if (currentDepth == targetDepth) {
        result.push_back({mNum, mDen});
        return;
    }

    /* Left subtree */
    enumerateAtDepth(lNum, lDen, mNum, mDen, currentDepth + 1, targetDepth, result);
    /* Right subtree */
    enumerateAtDepth(mNum, mDen, rNum, rDen, currentDepth + 1, targetDepth, result);
}

std::vector<Fraction> fractionsAtDepth(int depth) {
    std::vector<Fraction> result;
    if (depth == 0) {
        result.push_back({1, 1});
    } else {
        enumerateAtDepth(0, 1, 1, 0, 0, depth, result);
    }
    return result;
}

int64_t gcd(int64_t a, int64_t b) {
    a = std::abs(a);
    b = std::abs(b);
    while (b) { int64_t t = b; b = a % b; a = t; }
    return a;
}

} // namespace stern_brocot

int main() {
    std::cout << "========================================================\n";
    std::cout << "  Stern-Brocot Tree Navigator & Rational Approximation\n";
    std::cout << "========================================================\n\n";

    /* Find paths to known fractions */
    struct TestFrac { int64_t p, q; };
    std::vector<TestFrac> fracs = {{1,1}, {1,2}, {2,1}, {1,3}, {3,2}, {2,3}, {3,5}, {5,7}};

    std::cout << "Paths in the Stern-Brocot Tree:\n";
    std::cout << std::setw(10) << "Fraction" << std::setw(20) << "Path" << "\n";
    std::cout << "------------------------------\n";

    for (auto& f : fracs) {
        std::string path = stern_brocot::findPath(f.p, f.q);
        std::string fStr = std::to_string(f.p) + "/" + std::to_string(f.q);
        std::cout << std::setw(10) << fStr << std::setw(20)
                  << (path.empty() ? "(root)" : path) << "\n";
    }

    /* Best rational approximations */
    std::cout << "\nBest Rational Approximations (max denominator = 100):\n";
    std::cout << std::setw(16) << "Target" << std::setw(12) << "Approx"
              << std::setw(16) << "Value" << std::setw(14) << "Error" << "\n";
    std::cout << "----------------------------------------------------------\n";

    double targets[] = {3.14159265358979, 2.71828182845905, 1.41421356237310,
                        1.61803398874989, 0.33333333333333};
    std::string names[] = {"pi", "e", "sqrt(2)", "phi", "1/3"};

    for (int i = 0; i < 5; ++i) {
        auto approx = stern_brocot::bestApproximation(targets[i], 100);
        std::string fStr = std::to_string(approx.num) + "/" + std::to_string(approx.den);
        double err = std::abs(targets[i] - approx.value());

        std::cout << std::setw(10) << names[i] << std::setw(6) << " "
                  << std::setw(12) << fStr
                  << std::setw(16) << std::fixed << std::setprecision(10) << approx.value()
                  << std::setw(14) << std::scientific << std::setprecision(3) << err << "\n";
    }

    /* Enumerate fractions at depth 3 */
    std::cout << std::fixed << "\nFractions at depth 3 of the Stern-Brocot tree:\n  ";
    auto depth3 = stern_brocot::fractionsAtDepth(3);
    for (size_t i = 0; i < depth3.size(); ++i) {
        std::cout << depth3[i].num << "/" << depth3[i].den;
        if (i + 1 < depth3.size()) std::cout << ", ";
    }
    std::cout << "\n  Count: " << depth3.size() << " (expected 2^3 = 8)\n";

    return 0;
}
