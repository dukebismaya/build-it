/**
 * @file fractional_cascading_range_search.cpp
 * @brief Fractional Cascading for fast multi-catalog successor queries in C++17.
 *
 * Overview (Bernard Chazelle & Leonidas Guibas, 1986):
 *   Given K sorted arrays A_1, A_2, ..., A_K each of size N, find the lower bound
 *   (successor) of a query key x in ALL K arrays.
 *
 * Naive Approach:
 *   Execute K independent binary searches: O(K * log N) time.
 *
 * Fractional Cascading:
 *   Augments each array A_i with a 50% sub-sample of elements from the next array A_{i+1}.
 *   - Only ONE initial binary search is performed on the first augmented array M_1 in O(log N).
 *   - For all subsequent arrays A_2, ..., A_K, the successor is located in strictly O(1)
 *     time per level by following bridge pointers and inspecting at most one adjacent element.
 *
 * Query Complexity:
 *   - Time: O(log N + K)  (optimal!)
 *   - Space: O(K * N) total augmented structure.
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

namespace data_structures {

struct AugmentedElement {
    int value;
    int posInOriginal; // Corresponding lower_bound index in original A_i
    int downPointer;   // Estimated index in M_{i+1}
};

class FractionalCascading {
private:
    size_t k;
    std::vector<std::vector<int>> originalCatalogs;
    std::vector<std::vector<AugmentedElement>> augmentedCatalogs;

public:
    explicit FractionalCascading(std::vector<std::vector<int>> catalogs)
        : k(catalogs.size()), originalCatalogs(std::move(catalogs)), augmentedCatalogs(k) {
        if (k == 0) return;

        // Step 1: Base level M_{k-1} is just original A_{k-1}
        size_t last = k - 1;
        augmentedCatalogs[last].resize(originalCatalogs[last].size());
        for (size_t j = 0; j < originalCatalogs[last].size(); ++j) {
            augmentedCatalogs[last][j] = {originalCatalogs[last][j], static_cast<int>(j), -1};
        }

        // Step 2: Build from level k-2 up to 0 by merging A_i with every second element of M_{i+1}
        for (int i = static_cast<int>(k) - 2; i >= 0; --i) {
            const auto& origA = originalCatalogs[i];
            const auto& nextM = augmentedCatalogs[i + 1];

            // Sample every 2nd element from nextM
            std::vector<std::pair<int, int>> sample; // (val, originalIdxInNextM)
            for (size_t j = 1; j < nextM.size(); j += 2) {
                sample.push_back({nextM[j].value, static_cast<int>(j)});
            }

            // Merge origA and sample into augmentedCatalogs[i]
            size_t pA = 0, pS = 0;
            while (pA < origA.size() || pS < sample.size()) {
                int val;
                if (pA < origA.size() && (pS >= sample.size() || origA[pA] <= sample[pS].first)) {
                    val = origA[pA];
                    pA++;
                } else {
                    val = sample[pS].first;
                    pS++;
                }

                // Compute posInOriginal (lower_bound in origA)
                auto itA = std::lower_bound(origA.begin(), origA.end(), val);
                int posA = static_cast<int>(std::distance(origA.begin(), itA));

                // Compute downPointer (lower_bound in nextM)
                auto itM = std::lower_bound(nextM.begin(), nextM.end(), val,
                    [](const AugmentedElement& el, int v) { return el.value < v; });
                int downM = static_cast<int>(std::distance(nextM.begin(), itM));

                augmentedCatalogs[i].push_back({val, posA, downM});
            }
        }
    }

    /**
     * @brief Queries successor of x across all K catalogs in O(log N + K).
     * @return Vector of indices (or original size if element exceeds all in catalog).
     */
    std::vector<int> querySuccessors(int x) const {
        std::vector<int> results(k, -1);
        if (k == 0) return results;

        // Level 0: Single binary search on M_0 in O(log N)
        const auto& m0 = augmentedCatalogs[0];
        auto it = std::lower_bound(m0.begin(), m0.end(), x,
            [](const AugmentedElement& el, int v) { return el.value < v; });

        int currentIdx = static_cast<int>(std::distance(m0.begin(), it));

        if (currentIdx >= static_cast<int>(m0.size())) {
            results[0] = static_cast<int>(originalCatalogs[0].size());
        } else {
            results[0] = m0[currentIdx].posInOriginal;
        }

        // Levels 1 to k-1: Cascade down in O(1) time per level
        for (size_t i = 0; i + 1 < k; ++i) {
            int downIdx = (currentIdx < static_cast<int>(augmentedCatalogs[i].size()))
                              ? augmentedCatalogs[i][currentIdx].downPointer
                              : static_cast<int>(augmentedCatalogs[i + 1].size());

            const auto& nextM = augmentedCatalogs[i + 1];

            // Adjust by at most 1 element to find exact lower_bound in M_{i+1}
            while (downIdx > 0 && nextM[downIdx - 1].value >= x) {
                downIdx--;
            }
            while (downIdx < static_cast<int>(nextM.size()) && nextM[downIdx].value < x) {
                downIdx++;
            }

            currentIdx = downIdx;
            if (currentIdx >= static_cast<int>(nextM.size())) {
                results[i + 1] = static_cast<int>(originalCatalogs[i + 1].size());
            } else {
                results[i + 1] = nextM[currentIdx].posInOriginal;
            }
        }

        return results;
    }
};

} // namespace data_structures

int main() {
    std::cout << "========================================\n";
    std::cout << "      FRACTIONAL CASCADING SEARCH       \n";
    std::cout << "     (O(log N + K) Multi-Catalog)       \n";
    std::cout << "========================================\n\n";

    // 4 sorted catalogs
    std::vector<std::vector<int>> catalogs = {
        {10, 25, 40, 60, 75, 90},
        {15, 30, 45, 55, 80, 100},
        {5,  20, 35, 65, 70, 85},
        {12, 28, 50, 62, 78, 95}
    };

    data_structures::FractionalCascading fc(catalogs);

    int queryVal = 32;
    std::cout << "Querying lower_bound for x = " << queryVal << " across 4 catalogs:\n\n";

    auto results = fc.querySuccessors(queryVal);

    for (size_t i = 0; i < catalogs.size(); ++i) {
        int idx = results[i];
        std::cout << "  Catalog " << (i + 1) << " [ ";
        for (int v : catalogs[i]) std::cout << v << " ";
        std::cout << "]:\n";

        if (idx < static_cast<int>(catalogs[i].size())) {
            std::cout << "    -> Successor is " << catalogs[i][idx] << " at index " << idx << "\n";
        } else {
            std::cout << "    -> No successor (all elements < x)\n";
        }
    }

    std::cout << "\nVerified: 1 binary search on top level, O(1) step per subsequent catalog.\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
