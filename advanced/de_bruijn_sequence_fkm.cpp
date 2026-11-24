/*
 * ============================================================
 *    De Bruijn Sequences (FKM Algorithm + Eulerian Circuit)
 * ============================================================
 *
 * Description:
 *   A de Bruijn sequence B(k, n) is a cyclic string over an
 *   alphabet of size k in which every length-n string appears
 *   EXACTLY once as a (cyclic) substring. Its length is k^n.
 *
 *   Method 1 - Fredricksen-Kessler-Maiorana (FKM):
 *     Concatenate, in lexicographic order, all Lyndon words
 *     whose length divides n. The result is the lexicographically
 *     smallest de Bruijn sequence. Generated with the recursive
 *     "prenecklace" algorithm (Ruskey, Savage, Wang).
 *
 *   Method 2 - Eulerian circuit:
 *     Nodes = all (n-1)-length strings; each node has k outgoing
 *     edges (append a symbol). An Eulerian circuit (Hierholzer)
 *     of this graph spells a de Bruijn sequence.
 *
 * Application: shortest string that opens a k-symbol, n-digit
 *   combination lock with no "enter" key.
 *
 * Complexity: O(k^n) for both methods.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <functional>

namespace de_bruijn {

    std::string fkm(int k, int n) {
        std::vector<int> a(k * n + 1, 0);
        std::string seq;
        std::function<void(int, int)> db = [&](int t, int p) {
            if (t > n) {
                if (n % p == 0)
                    for (int i = 1; i <= p; ++i) seq += char('0' + a[i]);
            } else {
                a[t] = a[t - p];
                db(t + 1, p);
                for (int j = a[t - p] + 1; j < k; ++j) {
                    a[t] = j;
                    db(t + 1, t);
                }
            }
        };
        db(1, 1);
        return seq;
    }

    std::string eulerian(int k, int n) {
        if (n == 1) { std::string s; for (int i = 0; i < k; ++i) s += char('0' + i); return s; }
        int nodes = 1;
        for (int i = 0; i < n - 1; ++i) nodes *= k;
        std::vector<int> next_edge(nodes, 0);
        std::vector<int> stack{0}, circuit;
        while (!stack.empty()) {
            int v = stack.back();
            if (next_edge[v] < k) {
                int c = next_edge[v]++;
                stack.push_back((v * k + c) % nodes);
            } else {
                circuit.push_back(v);
                stack.pop_back();
            }
        }
        // circuit holds nodes in reverse; the symbol for each edge is the last digit of its head
        std::string seq;
        for (int i = static_cast<int>(circuit.size()) - 2; i >= 0; --i)
            seq += char('0' + circuit[i] % k);
        return seq;
    }

    bool verify(const std::string& s, int k, int n) {
        long long expected = 1;
        for (int i = 0; i < n; ++i) expected *= k;
        if (static_cast<long long>(s.size()) != expected) return false;
        std::string cyc = s + s.substr(0, n - 1);
        std::set<std::string> seen;
        for (size_t i = 0; i < s.size(); ++i) seen.insert(cyc.substr(i, n));
        return static_cast<long long>(seen.size()) == expected;
    }

} // namespace de_bruijn

int main() {
    using namespace de_bruijn;
    std::cout << "============================================================\n";
    std::cout << "    De Bruijn Sequences (FKM Algorithm + Eulerian Circuit)\n";
    std::cout << "============================================================\n\n";

    std::cout << "B(2,3) FKM      : " << fkm(2, 3) << "\n";
    std::cout << "B(2,3) Eulerian : " << eulerian(2, 3) << "\n";
    std::cout << "B(2,4) FKM      : " << fkm(2, 4) << "\n";
    std::cout << "B(3,2) FKM      : " << fkm(3, 2) << "\n";
    std::cout << "B(3,3) FKM      : " << fkm(3, 3) << "\n\n";

    bool ok = true;
    for (int k = 2; k <= 5; ++k)
        for (int n = 1; n <= 5; ++n) {
            long long len = 1; for (int i = 0; i < n; ++i) len *= k;
            if (len > 5000) continue;
            if (!verify(fkm(k, n), k, n) || !verify(eulerian(k, n), k, n)) ok = false;
        }
    std::cout << "Every length-n word appears exactly once (k<=5, n<=5): " << (ok ? "PASSED" : "FAILED") << "\n";

    std::string lock = fkm(10, 4);
    std::cout << "\nOpening a 4-digit keypad lock: de Bruijn length " << lock.size()
              << " + 3 wrap-around digits = " << lock.size() + 3
              << " presses (vs 40000 trying each code)\n";
    std::cout << "First 40 presses: " << lock.substr(0, 40) << "...\n";
    return 0;
}
