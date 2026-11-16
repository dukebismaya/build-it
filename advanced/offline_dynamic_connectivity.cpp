/**
 * ============================================================================
 * Offline Dynamic Connectivity (Segment Tree over Time with DSU Rollback)
 * ============================================================================
 * 
 * Description:
 * Solves the dynamic graph connectivity problem offline:
 * Given a graph of V vertices and Q queries/updates:
 *   1. Add edge (u, v)
 *   2. Remove edge (u, v)
 *   3. Query if u and v are connected
 * 
 * Algorithm:
 * 1. Lifetime Intervals:
 *    Each edge e exists over a set of time intervals [t_start, t_end].
 *    If an edge is added and never removed, its interval is [t_add, Q].
 * 
 * 2. Segment Tree over Time:
 *    We build a segment tree over the timeline [1, Q].
 *    Each edge's lifetime [t_start, t_end] is inserted into O(log Q) canonical
 *    nodes of the segment tree.
 * 
 * 3. DSU with Rollback (Undo):
 *    Standard DSU with union-by-rank, but WITHOUT path compression (to allow
 *    O(1) rollback).
 *    Each union operation records modified nodes in a history stack.
 *    Rolling back k operations restores the exact previous state in O(k).
 * 
 * 4. Divide-and-Conquer / Segment Tree DFS:
 *    - Visit node: Apply all edge unions stored at the node.
 *    - If leaf node at time t: If query at time t exists, answer whether
 *      find(u) == find(v).
 *    - Recurse on children.
 *    - On exit: Rollback all unions made at this node.
 * 
 * Time Complexity:
 * - O(Q log Q log V) overall runtime
 * - Space Complexity: O(V + Q log Q)
 * ============================================================================
 */

#include <iostream>
#include <vector>
#include <map>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace dynamic_connectivity {

struct Edge {
    int u, v;
    bool operator<(const Edge& other) const {
        if (u != other.u) return u < other.u;
        return v < other.v;
    }
};

enum QueryType {
    ADD_EDGE,
    REMOVE_EDGE,
    IS_CONNECTED
};

struct Event {
    QueryType type;
    int u, v;
};

// DSU with union-by-rank and rollback capability
class RollbackDSU {
public:
    int n;
    std::vector<int> parent;
    std::vector<int> rank;

    struct Change {
        int u, v;
        int prev_rank_v;
    };
    std::vector<Change> history;

    explicit RollbackDSU(int num_nodes) : n(num_nodes), parent(num_nodes), rank(num_nodes, 0) {
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int find(int x) const {
        while (x != parent[x]) {
            x = parent[x];
        }
        return x;
    }

    bool unite(int u, int v) {
        int root_u = find(u);
        int root_v = find(v);
        if (root_u == root_v) {
            return false;
        }

        if (rank[root_u] < rank[root_v]) {
            std::swap(root_u, root_v);
        }

        // Attach root_v under root_u
        history.push_back({root_u, root_v, rank[root_u]});
        parent[root_v] = root_u;
        if (rank[root_u] == rank[root_v]) {
            rank[root_u]++;
        }
        return true;
    }

    void rollback(size_t checkpoint) {
        while (history.size() > checkpoint) {
            Change c = history.back();
            history.pop_back();
            parent[c.v] = c.v;
            rank[c.u] = c.prev_rank_v;
        }
    }

    bool connected(int u, int v) const {
        return find(u) == find(v);
    }
};

class OfflineDynamicConnectivity {
public:
    int num_vertices;
    int num_queries;
    std::vector<Event> events;

    std::vector<std::vector<Edge>> seg_tree;
    std::vector<bool> answers;

    OfflineDynamicConnectivity(int v_count, const std::vector<Event>& evs)
        : num_vertices(v_count), num_queries(static_cast<int>(evs.size())), events(evs) {
        seg_tree.resize(4 * (num_queries + 1));
        answers.assign(num_queries, false);
    }

    void insertInterval(int node, int l, int r, int ql, int qr, const Edge& e) {
        if (ql <= l && r <= qr) {
            seg_tree[node].push_back(e);
            return;
        }
        int mid = (l + r) / 2;
        if (ql <= mid) insertInterval(2 * node, l, mid, ql, qr, e);
        if (qr > mid)  insertInterval(2 * node + 1, mid + 1, r, ql, qr, e);
    }

    void solveDFS(int node, int l, int r, RollbackDSU& dsu) {
        size_t checkpoint = dsu.history.size();

        // Apply all edges at current segment tree node
        for (const auto& edge : seg_tree[node]) {
            dsu.unite(edge.u, edge.v);
        }

        if (l == r) {
            int q_idx = l - 1; // 0-based index
            if (q_idx < num_queries && events[q_idx].type == IS_CONNECTED) {
                answers[q_idx] = dsu.connected(events[q_idx].u, events[q_idx].v);
            }
        } else {
            int mid = (l + r) / 2;
            solveDFS(2 * node, l, mid, dsu);
            solveDFS(2 * node + 1, mid + 1, r, dsu);
        }

        // Rollback edges added at this node
        dsu.rollback(checkpoint);
    }

    std::vector<bool> solve() {
        if (num_queries == 0) return answers;

        // Map each edge to its start time
        std::map<Edge, int> active_edges;

        for (int i = 0; i < num_queries; ++i) {
            int time_stamp = i + 1; // 1-based
            int u = events[i].u;
            int v = events[i].v;
            if (u > v) std::swap(u, v);
            Edge e{u, v};

            if (events[i].type == ADD_EDGE) {
                active_edges[e] = time_stamp;
            } else if (events[i].type == REMOVE_EDGE) {
                if (active_edges.count(e)) {
                    int start_time = active_edges[e];
                    int end_time = time_stamp;
                    insertInterval(1, 1, num_queries, start_time, end_time, e);
                    active_edges.erase(e);
                }
            }
        }

        // Edges that remain active until the end
        for (const auto& pair : active_edges) {
            int start_time = pair.second;
            int end_time = num_queries;
            insertInterval(1, 1, num_queries, start_time, end_time, pair.first);
        }

        RollbackDSU dsu(num_vertices);
        solveDFS(1, 1, num_queries, dsu);

        return answers;
    }
};

} // namespace dynamic_connectivity

int main() {
    std::cout << "========================================================\n";
    std::cout << " Offline Dynamic Connectivity (Segment Tree & DSU)\n";
    std::cout << "========================================================\n\n";

    int num_vertices = 5;
    std::vector<dynamic_connectivity::Event> events = {
        {dynamic_connectivity::ADD_EDGE, 0, 1},     // t=1: Add 0-1
        {dynamic_connectivity::ADD_EDGE, 1, 2},     // t=2: Add 1-2
        {dynamic_connectivity::IS_CONNECTED, 0, 2}, // t=3: Query 0-2 (expected YES)
        {dynamic_connectivity::IS_CONNECTED, 0, 3}, // t=4: Query 0-3 (expected NO)
        {dynamic_connectivity::ADD_EDGE, 2, 3},     // t=5: Add 2-3
        {dynamic_connectivity::IS_CONNECTED, 0, 3}, // t=6: Query 0-3 (expected YES)
        {dynamic_connectivity::REMOVE_EDGE, 1, 2},  // t=7: Remove 1-2 (disconnects {0,1} from {2,3})
        {dynamic_connectivity::IS_CONNECTED, 0, 3}, // t=8: Query 0-3 (expected NO)
        {dynamic_connectivity::IS_CONNECTED, 2, 3}, // t=9: Query 2-3 (expected YES)
        {dynamic_connectivity::ADD_EDGE, 0, 3},     // t=10: Add 0-3 directly
        {dynamic_connectivity::IS_CONNECTED, 0, 3}  // t=11: Query 0-3 (expected YES)
    };

    std::cout << "Processing " << events.size() << " dynamic events over "
              << num_vertices << " vertices...\n\n";

    dynamic_connectivity::OfflineDynamicConnectivity solver(num_vertices, events);
    std::vector<bool> answers = solver.solve();

    std::cout << "Event Trace & Query Answers:\n";
    std::cout << "--------------------------------------------------------\n";
    std::cout << std::setw(6) << "Time"
              << std::setw(15) << "Event Type"
              << std::setw(12) << "Edge/Pair"
              << std::setw(15) << "Connectivity\n";
    std::cout << "--------------------------------------------------------\n";

    for (size_t i = 0; i < events.size(); ++i) {
        int t = static_cast<int>(i + 1);
        std::string type_str;
        if (events[i].type == dynamic_connectivity::ADD_EDGE) type_str = "ADD_EDGE";
        else if (events[i].type == dynamic_connectivity::REMOVE_EDGE) type_str = "REMOVE_EDGE";
        else type_str = "IS_CONNECTED";

        std::string pair_str = "(" + std::to_string(events[i].u) + ", " + std::to_string(events[i].v) + ")";

        std::cout << std::setw(6) << t
                  << std::setw(15) << type_str
                  << std::setw(12) << pair_str;

        if (events[i].type == dynamic_connectivity::IS_CONNECTED) {
            std::cout << std::setw(15) << (answers[i] ? "CONNECTED (YES)" : "DISCONNECTED (NO)");
        } else {
            std::cout << std::setw(15) << "-";
        }
        std::cout << "\n";
    }

    std::cout << "--------------------------------------------------------\n";
    std::cout << "All queries successfully answered in O(Q log Q log V)!\n";
    return 0;
}
