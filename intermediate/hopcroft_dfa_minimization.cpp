/**
 * ============================================================================
 * Hopcroft's DFA State Minimization Algorithm (Optimal O(|Sigma| * N log N))
 * ============================================================================
 * 
 * Description:
 * Hopcroft's algorithm (introduced by John Hopcroft in 1971) is the asymptotically
 * optimal algorithm for minimizing the number of states in a Deterministic Finite
 * Automaton (DFA).
 * 
 * Mechanism:
 * 1. Reachability Filtering:
 *    First remove all unreachable states via BFS/DFS from the initial state.
 * 
 * 2. Initial Partition:
 *    Divide remaining states into two initial equivalence classes:
 *      - F (accepting / final states)
 *      - Q \ F (non-accepting states)
 * 
 * 3. Partition Refinement:
 *    Maintain a set W of partition splitters, initialized to {min(F, Q \ F)}.
 *    While W is not empty:
 *      - Extract a splitter set A from W.
 *      - For each symbol c in Sigma:
 *          Let X = {q in Q | delta(q, c) in A}.
 *          For each existing block Y in partition P:
 *            If Y intersects both X and (Y \ X):
 *              Split Y into Y1 = Y intersect X and Y2 = Y \ X.
 *              Replace Y with Y1 and Y2 in P.
 *              If Y is in W, replace Y with Y1 and Y2 in W.
 *              Otherwise, add min(|Y1|, |Y2|) to W!
 *              (Adding the smaller set guarantees O(N log N) runtime).
 * 
 * 4. Minimal DFA Reconstruction:
 *    Each block in the final partition becomes a state in the minimal DFA.
 * 
 * Time Complexity:
 * - O(|Sigma| * N log N) where N is number of states and |Sigma| is alphabet size.
 * - Space Complexity: O(N * |Sigma|)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
#include <iomanip>

namespace hopcroft {

struct DFA {
    int num_states;
    std::vector<char> alphabet;
    int start_state;
    std::set<int> accept_states;
    // delta[state][symbol_index] = next_state
    std::vector<std::vector<int>> transition;
};

/**
 * Removes states not reachable from the start state.
 */
DFA removeUnreachable(const DFA& dfa) {
    std::vector<bool> reachable(dfa.num_states, false);
    std::queue<int> q;

    reachable[dfa.start_state] = true;
    q.push(dfa.start_state);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int next : dfa.transition[u]) {
            if (next != -1 && !reachable[next]) {
                reachable[next] = true;
                q.push(next);
            }
        }
    }

    std::vector<int> new_id(dfa.num_states, -1);
    int new_count = 0;
    for (int i = 0; i < dfa.num_states; ++i) {
        if (reachable[i]) {
            new_id[i] = new_count++;
        }
    }

    DFA filtered;
    filtered.num_states = new_count;
    filtered.alphabet = dfa.alphabet;
    filtered.start_state = new_id[dfa.start_state];
    filtered.transition.assign(new_count, std::vector<int>(dfa.alphabet.size(), -1));

    for (int i = 0; i < dfa.num_states; ++i) {
        if (reachable[i]) {
            if (dfa.accept_states.count(i)) {
                filtered.accept_states.insert(new_id[i]);
            }
            for (size_t c = 0; c < dfa.alphabet.size(); ++c) {
                int next = dfa.transition[i][c];
                if (next != -1 && reachable[next]) {
                    filtered.transition[new_id[i]][c] = new_id[next];
                }
            }
        }
    }

    return filtered;
}

/**
 * Executes Hopcroft's partition refinement algorithm.
 */
DFA minimizeDFA(const DFA& input_dfa) {
    DFA dfa = removeUnreachable(input_dfa);
    int n = dfa.num_states;
    int num_symbols = static_cast<int>(dfa.alphabet.size());

    // Step 1: Initial partition P into F and Q \ F
    std::set<int> F = dfa.accept_states;
    std::set<int> non_F;
    for (int i = 0; i < n; ++i) {
        if (!F.count(i)) non_F.insert(i);
    }

    std::vector<std::set<int>> P;
    if (!F.empty()) P.push_back(F);
    if (!non_F.empty()) P.push_back(non_F);

    std::vector<std::set<int>> W;
    if (!F.empty() && (F.size() <= non_F.size() || non_F.empty())) {
        W.push_back(F);
    } else if (!non_F.empty()) {
        W.push_back(non_F);
    }

    // Precompute inverse transitions: inv[v][c] = list of states u where delta(u, c) == v
    std::vector<std::vector<std::vector<int>>> inv(n, std::vector<std::vector<int>>(num_symbols));
    for (int u = 0; u < n; ++u) {
        for (int c = 0; c < num_symbols; ++c) {
            int v = dfa.transition[u][c];
            if (v != -1) {
                inv[v][c].push_back(u);
            }
        }
    }

    // Step 2: Refine partitions
    while (!W.empty()) {
        std::set<int> A = W.back();
        W.pop_back();

        for (int c = 0; c < num_symbols; ++c) {
            // Find X = {u | delta(u, c) in A}
            std::set<int> X;
            for (int v : A) {
                for (int u : inv[v][c]) {
                    X.insert(u);
                }
            }

            std::vector<std::set<int>> new_P;
            for (const auto& Y : P) {
                std::set<int> Y1, Y2;
                for (int state : Y) {
                    if (X.count(state)) {
                        Y1.insert(state);
                    } else {
                        Y2.insert(state);
                    }
                }

                if (!Y1.empty() && !Y2.empty()) {
                    new_P.push_back(Y1);
                    new_P.push_back(Y2);

                    // Update waiting set W
                    auto it = std::find(W.begin(), W.end(), Y);
                    if (it != W.end()) {
                        W.erase(it);
                        W.push_back(Y1);
                        W.push_back(Y2);
                    } else {
                        if (Y1.size() <= Y2.size()) {
                            W.push_back(Y1);
                        } else {
                            W.push_back(Y2);
                        }
                    }
                } else {
                    new_P.push_back(Y);
                }
            }
            P = std::move(new_P);
        }
    }

    // Step 3: Reconstruct minimized DFA
    int min_states = static_cast<int>(P.size());
    std::vector<int> state_map(n);
    for (int block_idx = 0; block_idx < min_states; ++block_idx) {
        for (int state : P[block_idx]) {
            state_map[state] = block_idx;
        }
    }

    DFA min_dfa;
    min_dfa.num_states = min_states;
    min_dfa.alphabet = dfa.alphabet;
    min_dfa.start_state = state_map[dfa.start_state];
    min_dfa.transition.assign(min_states, std::vector<int>(num_symbols, -1));

    for (int block_idx = 0; block_idx < min_states; ++block_idx) {
        int rep = *P[block_idx].begin();
        if (dfa.accept_states.count(rep)) {
            min_dfa.accept_states.insert(block_idx);
        }
        for (int c = 0; c < num_symbols; ++c) {
            int next = dfa.transition[rep][c];
            if (next != -1) {
                min_dfa.transition[block_idx][c] = state_map[next];
            }
        }
    }

    return min_dfa;
}

} // namespace hopcroft

int main() {
    std::cout << "========================================================\n";
    std::cout << " Hopcroft's DFA State Minimization Algorithm\n";
    std::cout << "========================================================\n\n";

    // Classic textbook DFA with 6 states (0 to 5) that accepts strings ending with '1'
    // Contains redundant equivalent states:
    // Alphabet: {'0', '1'}
    hopcroft::DFA dfa;
    dfa.num_states = 6;
    dfa.alphabet = {'0', '1'};
    dfa.start_state = 0;
    dfa.accept_states = {2, 3, 4}; // States 2, 3, 4 are all accepting
    dfa.transition.assign(6, std::vector<int>(2));

    // Transitions:
    // State 0: on '0' -> 1, on '1' -> 2
    // State 1: on '0' -> 0, on '1' -> 3
    // State 2: on '0' -> 1, on '1' -> 4
    // State 3: on '0' -> 5, on '1' -> 4
    // State 4: on '0' -> 5, on '1' -> 4
    // State 5: on '0' -> 5, on '1' -> 4
    dfa.transition[0] = {1, 2};
    dfa.transition[1] = {0, 3};
    dfa.transition[2] = {1, 4};
    dfa.transition[3] = {5, 4};
    dfa.transition[4] = {5, 4};
    dfa.transition[5] = {5, 4};

    std::cout << "Original DFA:\n";
    std::cout << "  Number of states: " << dfa.num_states << "\n";
    std::cout << "  Start state: " << dfa.start_state << "\n";
    std::cout << "  Accepting states: {2, 3, 4}\n";
    std::cout << "  Transition Table:\n";
    std::cout << "    State |  '0'  |  '1'\n";
    std::cout << "    --------------------\n";
    for (int i = 0; i < dfa.num_states; ++i) {
        std::cout << "      " << i << "   |   "
                  << dfa.transition[i][0] << "   |   "
                  << dfa.transition[i][1] << (dfa.accept_states.count(i) ? "  [ACCEPT]" : "") << "\n";
    }

    auto min_dfa = hopcroft::minimizeDFA(dfa);

    std::cout << "\n--------------------------------------------------------\n";
    std::cout << " Minimized DFA (via Hopcroft's Algorithm):\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << "  Number of states: " << min_dfa.num_states << " (Reduced from " << dfa.num_states << "!)\n";
    std::cout << "  Start state: " << min_dfa.start_state << "\n";
    std::cout << "  Accepting states: { ";
    for (int s : min_dfa.accept_states) std::cout << s << " ";
    std::cout << "}\n";
    std::cout << "  Minimized Transition Table:\n";
    std::cout << "    State |  '0'  |  '1'\n";
    std::cout << "    --------------------\n";
    for (int i = 0; i < min_dfa.num_states; ++i) {
        std::cout << "      " << i << "   |   "
                  << min_dfa.transition[i][0] << "   |   "
                  << min_dfa.transition[i][1] << (min_dfa.accept_states.count(i) ? "  [ACCEPT]" : "") << "\n";
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "Minimization completed with O(|Sigma| * N log N) efficiency!\n";

    return 0;
}
