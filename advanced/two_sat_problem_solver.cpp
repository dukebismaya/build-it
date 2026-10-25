/*
 * Program Description:
 * 2-SAT (2-Satisfiability) Problem Solver in C++.
 * Determines whether a boolean formula in 2-CNF (Conjunctive Normal Form where each
 * clause contains at most 2 literals) can be satisfied, and finds a valid truth assignment.
 *
 * Implication Graph Construction:
 * A clause (A or B) is equivalent to two implications:
 * (!A -> B) and (!B -> A).
 * Each variable x has two nodes: x and !x.
 *
 * Algorithm (Aspvall, Plass & Tarjan / Kosaraju's SCC):
 * 1. Build the directed implication graph.
 * 2. Find Strongly Connected Components (SCC) using Kosaraju's algorithm in O(V + E).
 * 3. A 2-CNF formula is satisfiable IF AND ONLY IF no variable x and its negation !x
 *    belong to the same SCC.
 * 4. Truth assignment: If scc[x] < scc[!x], then x is true; otherwise false.
 *
 * Time Complexity: O(Variables + Clauses)
 * Space Complexity: O(Variables + Clauses)
 */

#include <iostream>
#include <vector>
#include <stack>
#include <string>

class TwoSatSolver {
    int numVars;
    int numNodes;
    std::vector<std::vector<int>> adj;
    std::vector<std::vector<int>> adjRev;
    std::vector<bool> visited;
    std::stack<int> order;
    std::vector<int> scc;

    // Node representation:
    // Positive literal x (1 <= x <= N): 2 * (x - 1)
    // Negative literal !x: 2 * (x - 1) + 1
    int toNode(int lit) const {
        if (lit > 0) return 2 * (lit - 1);
        return 2 * (-lit - 1) + 1;
    }

    int negateNode(int u) const {
        return u ^ 1;
    }

    void dfs1(int u) {
        visited[u] = true;
        for (int v : adj[u]) {
            if (!visited[v]) dfs1(v);
        }
        order.push(u);
    }

    void dfs2(int u, int componentId) {
        scc[u] = componentId;
        for (int v : adjRev[u]) {
            if (scc[v] == -1) dfs2(v, componentId);
        }
    }

public:
    explicit TwoSatSolver(int n) : numVars(n), numNodes(2 * n), adj(2 * n), adjRev(2 * n) {}

    // Adds clause (u or v). Positive integer means variable, negative means negation.
    void addClause(int uLit, int vLit) {
        int u = toNode(uLit);
        int v = toNode(vLit);

        // (!u -> v)
        adj[negateNode(u)].push_back(v);
        adjRev[v].push_back(negateNode(u));

        // (!v -> u)
        adj[negateNode(v)].push_back(u);
        adjRev[u].push_back(negateNode(v));
    }

    // Solves 2-SAT and returns pair <isSatisfiable, assignmentVector>
    std::pair<bool, std::vector<bool>> solve() {
        visited.assign(numNodes, false);
        while (!order.empty()) order.pop();

        for (int i = 0; i < numNodes; ++i) {
            if (!visited[i]) dfs1(i);
        }

        scc.assign(numNodes, -1);
        int componentCount = 0;

        while (!order.empty()) {
            int u = order.top();
            order.pop();
            if (scc[u] == -1) {
                dfs2(u, componentCount++);
            }
        }

        std::vector<bool> assignment(numVars + 1, false);

        for (int i = 1; i <= numVars; ++i) {
            int pos = toNode(i);
            int neg = toNode(-i);

            if (scc[pos] == scc[neg]) {
                return {false, {}}; // Contradiction: x and !x in same SCC
            }

            // In topological ordering of SCCs, earlier component is reachable from later
            assignment[i] = (scc[pos] > scc[neg]);
        }

        return {true, assignment};
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "        2-SAT Problem Solver (O(V+E))     \n";
    std::cout << "==========================================\n\n";

    // Formula 1: Satisfiable
    // (x1 or x2) and (!x1 or x3) and (!x2 or !x3)
    TwoSatSolver solver1(3);
    solver1.addClause(1, 2);
    solver1.addClause(-1, 3);
    solver1.addClause(-2, -3);

    std::cout << "Formula 1: (x1 v x2) ^ (!x1 v x3) ^ (!x2 v !x3)\n";
    auto [sat1, assign1] = solver1.solve();
    if (sat1) {
        std::cout << "  Status: SATISFIABLE [PASS]\n";
        std::cout << "  Truth Assignment:\n";
        for (int i = 1; i <= 3; ++i) {
            std::cout << "    x" << i << " = " << (assign1[i] ? "TRUE" : "FALSE") << "\n";
        }
    } else {
        std::cout << "  Status: UNSATISFIABLE\n";
    }

    std::cout << "\n------------------------------------------\n\n";

    // Formula 2: Unsatisfiable
    // (x1 or x1) and (!x1 or !x1) => x1 is both TRUE and FALSE
    TwoSatSolver solver2(1);
    solver2.addClause(1, 1);
    solver2.addClause(-1, -1);

    std::cout << "Formula 2: (x1 v x1) ^ (!x1 v !x1)\n";
    auto [sat2, assign2] = solver2.solve();
    std::cout << "  Status: " << (sat2 ? "SATISFIABLE" : "UNSATISFIABLE [CORRECT CONTRADICTION]") << "\n";

    return 0;
}
