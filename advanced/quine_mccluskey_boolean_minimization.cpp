/**
 * @file quine_mccluskey_boolean_minimization.cpp
 * @brief Quine-McCluskey Tabular Boolean Function Minimization Algorithm in C++17.
 *
 * Overview (Willard V. Quine, 1952 & Edward J. McCluskey, 1956):
 *   A systematic tabular algorithm for minimizing Sum-of-Products (SOP) Boolean functions.
 *   It provides an exact alternative to Karnaugh maps that scales algorithmically to
 *   more than 4 variables.
 *
 * Two-Phase Architecture:
 *   Phase 1: Prime Implicant (PI) Generation
 *     - Group minterms and don't-cares by Hamming weight (number of 1 bits).
 *     - Repeatedly merge pairs of implicants sharing the same dash mask that differ
 *       in exactly one bit position, placing a don't-care dash '-' at that position.
 *     - Unmerged implicants constitute the complete set of Prime Implicants.
 *
 *   Phase 2: Prime Implicant Chart & Essential Prime Implicants (EPI)
 *     - Construct coverage chart mapping each required minterm to covering PIs.
 *     - Extract Essential Prime Implicants (PIs that are the sole cover for at least one minterm).
 *     - Cover any residual uncovered minterms via exact minimum set cover search.
 *
 * Complexity:
 *   - Worst-case exponential O(3^n / sqrt(n)) in number of variables n,
 *     as a Boolean function can have exponentially many prime implicants.
 */

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <iomanip>
#include <cstdint>

namespace boolean_logic {

struct Implicant {
    int bits;
    int mask; // Bit = 1 means '-' (eliminated variable)
    bool combined;
    std::set<int> minterms;

    bool operator<(const Implicant& other) const {
        if (mask != other.mask) return mask < other.mask;
        return bits < other.bits;
    }

    bool operator==(const Implicant& other) const {
        return bits == other.bits && mask == other.mask;
    }
};

int countSetBits(int x) {
    int cnt = 0;
    while (x) {
        cnt += (x & 1);
        x >>= 1;
    }
    return cnt;
}

std::string implicantToBinaryString(const Implicant& imp, int numVars) {
    std::string s;
    for (int i = numVars - 1; i >= 0; --i) {
        if ((imp.mask >> i) & 1) {
            s += '-';
        } else {
            s += ((imp.bits >> i) & 1) ? '1' : '0';
        }
    }
    return s;
}

std::string implicantToExpression(const Implicant& imp, int numVars) {
    std::string expr;
    for (int i = numVars - 1; i >= 0; --i) {
        if (!((imp.mask >> i) & 1)) {
            char varName = static_cast<char>('A' + (numVars - 1 - i));
            expr += varName;
            if (!((imp.bits >> i) & 1)) {
                expr += '\'';
            }
        }
    }
    return expr.empty() ? "1" : expr;
}

class QuineMcCluskey {
private:
    int numVars;
    std::vector<int> minterms;
    std::vector<int> dontCares;

    void findMinCoverRecursive(const std::vector<Implicant>& remainingPIs,
                               const std::set<int>& uncovered,
                               std::vector<Implicant>& currentChoice,
                               std::vector<Implicant>& bestChoice) {
        if (uncovered.empty()) {
            if (bestChoice.empty() || currentChoice.size() < bestChoice.size()) {
                bestChoice = currentChoice;
            }
            return;
        }
        if (!bestChoice.empty() && currentChoice.size() >= bestChoice.size()) {
            return; // Branch & bound prune
        }

        // Pick first uncovered minterm
        int target = *uncovered.begin();

        for (const auto& pi : remainingPIs) {
            if (pi.minterms.count(target)) {
                std::set<int> nextUncovered = uncovered;
                for (int m : pi.minterms) {
                    nextUncovered.erase(m);
                }
                currentChoice.push_back(pi);
                findMinCoverRecursive(remainingPIs, nextUncovered, currentChoice, bestChoice);
                currentChoice.pop_back();
            }
        }
    }

public:
    QuineMcCluskey(int variables, std::vector<int> m, std::vector<int> dc = {})
        : numVars(variables), minterms(std::move(m)), dontCares(std::move(dc)) {}

    struct Result {
        std::vector<Implicant> primeImplicants;
        std::vector<Implicant> essentialPIs;
        std::vector<Implicant> minimalCover;
        std::string minimizedSOP;
    };

    Result minimize() {
        // Combine minterms and don't-cares for Phase 1
        std::set<int> allTerms(minterms.begin(), minterms.end());
        allTerms.insert(dontCares.begin(), dontCares.end());

        std::vector<Implicant> currentGroup;
        for (int t : allTerms) {
            currentGroup.push_back({t, 0, false, {t}});
        }

        std::set<Implicant> primeImplicantsSet;

        while (!currentGroup.empty()) {
            // Group by Hamming weight (number of 1-bits)
            std::map<int, std::vector<size_t>> buckets;
            for (size_t i = 0; i < currentGroup.size(); ++i) {
                int activeBits = currentGroup[i].bits & (~currentGroup[i].mask);
                buckets[countSetBits(activeBits)].push_back(i);
            }

            std::vector<Implicant> nextGroup;
            std::set<std::pair<int, int>> seenNext;

            for (auto it = buckets.begin(); it != buckets.end(); ++it) {
                int weight = it->first;
                if (buckets.find(weight + 1) == buckets.end()) continue;

                const auto& g1 = it->second;
                const auto& g2 = buckets[weight + 1];

                for (size_t idx1 : g1) {
                    for (size_t idx2 : g2) {
                        Implicant& imp1 = currentGroup[idx1];
                        Implicant& imp2 = currentGroup[idx2];

                        if (imp1.mask != imp2.mask) continue;

                        int diff = (imp1.bits ^ imp2.bits) & (~imp1.mask);
                        if (diff != 0 && (diff & (diff - 1)) == 0) {
                            // Differ by exact 1 bit! Combine them
                            imp1.combined = true;
                            imp2.combined = true;

                            int newMask = imp1.mask | diff;
                            int newBits = imp1.bits & (~newMask);

                            if (seenNext.find({newBits, newMask}) == seenNext.end()) {
                                seenNext.insert({newBits, newMask});
                                std::set<int> mergedTerms = imp1.minterms;
                                mergedTerms.insert(imp2.minterms.begin(), imp2.minterms.end());
                                nextGroup.push_back({newBits, newMask, false, mergedTerms});
                            }
                        }
                    }
                }
            }

            // Collect uncombined implicants as Prime Implicants
            for (const auto& imp : currentGroup) {
                if (!imp.combined) {
                    primeImplicantsSet.insert(imp);
                }
            }

            currentGroup = std::move(nextGroup);
        }

        std::vector<Implicant> primeImplicants(primeImplicantsSet.begin(), primeImplicantsSet.end());

        // Phase 2: Prime Implicant Chart (only consider required minterms, NOT don't-cares)
        std::set<int> reqMinterms(minterms.begin(), minterms.end());
        std::set<int> uncovered = reqMinterms;
        std::vector<Implicant> essentialPIs;
        std::vector<bool> usedPI(primeImplicants.size(), false);

        for (int m : reqMinterms) {
            std::vector<size_t> covering;
            for (size_t i = 0; i < primeImplicants.size(); ++i) {
                if (primeImplicants[i].minterms.count(m)) {
                    covering.push_back(i);
                }
            }
            if (covering.size() == 1) {
                size_t epiIdx = covering[0];
                if (!usedPI[epiIdx]) {
                    usedPI[epiIdx] = true;
                    essentialPIs.push_back(primeImplicants[epiIdx]);
                    for (int covM : primeImplicants[epiIdx].minterms) {
                        uncovered.erase(covM);
                    }
                }
            }
        }

        // Handle remaining uncovered minterms
        std::vector<Implicant> remainingPIs;
        for (size_t i = 0; i < primeImplicants.size(); ++i) {
            if (!usedPI[i]) {
                remainingPIs.push_back(primeImplicants[i]);
            }
        }

        std::vector<Implicant> minimalCover = essentialPIs;
        if (!uncovered.empty()) {
            std::vector<Implicant> currentChoice;
            std::vector<Implicant> bestExtra;
            findMinCoverRecursive(remainingPIs, uncovered, currentChoice, bestExtra);
            minimalCover.insert(minimalCover.end(), bestExtra.begin(), bestExtra.end());
        }

        // Format minimized SOP expression
        std::string sop;
        for (size_t i = 0; i < minimalCover.size(); ++i) {
            sop += implicantToExpression(minimalCover[i], numVars);
            if (i + 1 < minimalCover.size()) sop += " + ";
        }

        return {primeImplicants, essentialPIs, minimalCover, sop};
    }
};

} // namespace boolean_logic

int main() {
    std::cout << "========================================\n";
    std::cout << " QUINE-MCCLUSKEY BOOLEAN MINIMIZATION   \n";
    std::cout << "========================================\n\n";

    // Classic 4-variable function F(A, B, C, D) = sum m(0, 1, 2, 5, 6, 7, 8, 9, 10, 14)
    int numVars = 4;
    std::vector<int> minterms = {0, 1, 2, 5, 6, 7, 8, 9, 10, 14};
    std::vector<int> dontCares = {};

    std::cout << "Boolean Function F(A, B, C, D) = sum m(";
    for (size_t i = 0; i < minterms.size(); ++i) {
        std::cout << minterms[i] << (i + 1 == minterms.size() ? "" : ", ");
    }
    std::cout << ")\n\n";

    boolean_logic::QuineMcCluskey qm(numVars, minterms, dontCares);
    auto res = qm.minimize();

    std::cout << "Phase 1 - All Prime Implicants (" << res.primeImplicants.size() << "):\n";
    for (const auto& pi : res.primeImplicants) {
        std::cout << "  " << std::setw(6) << boolean_logic::implicantToBinaryString(pi, numVars)
                  << " (" << std::setw(5) << boolean_logic::implicantToExpression(pi, numVars) << ") covers: { ";
        for (int m : pi.minterms) std::cout << m << " ";
        std::cout << "}\n";
    }

    std::cout << "\nPhase 2 - Essential Prime Implicants (" << res.essentialPIs.size() << "):\n";
    for (const auto& epi : res.essentialPIs) {
        std::cout << "  - " << boolean_logic::implicantToExpression(epi, numVars)
                  << " [" << boolean_logic::implicantToBinaryString(epi, numVars) << "]\n";
    }

    std::cout << "\nMinimized Sum-of-Products (SOP) Expression:\n";
    std::cout << "  F(A, B, C, D) = " << res.minimizedSOP << "\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
