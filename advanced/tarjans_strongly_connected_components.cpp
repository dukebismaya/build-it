/*
 * Problem Description:
 * Implement Tarjan's strongly connected components algorithm.
 * It finds all strongly connected components in a directed graph in linear time.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

class Graph {
    int V;
    vector<vector<int>> adj;
    void SCCUtil(int u, vector<int>& disc, vector<int>& low, stack<int>& st, vector<bool>& stackMember);

public:
    Graph(int V);
    void addEdge(int v, int w);
    void SCC();
};

Graph::Graph(int V) {
    this->V = V;
    adj.resize(V);
}

void Graph::addEdge(int v, int w) {
    adj[v].push_back(w);
}

void Graph::SCCUtil(int u, vector<int>& disc, vector<int>& low, stack<int>& st, vector<bool>& stackMember) {
    static int time = 0;
    disc[u] = low[u] = ++time;
    st.push(u);
    stackMember[u] = true;

    for (int v : adj[u]) {
        if (disc[v] == -1) {
            SCCUtil(v, disc, low, st, stackMember);
            low[u] = min(low[u], low[v]);
        }
        else if (stackMember[v] == true) {
            low[u] = min(low[u], disc[v]);
        }
    }

    int w = 0;
    if (low[u] == disc[u]) {
        while (st.top() != u) {
            w = st.top();
            cout << w << " ";
            stackMember[w] = false;
            st.pop();
        }
        w = st.top();
        cout << w << "\n";
        stackMember[w] = false;
        st.pop();
    }
}

void Graph::SCC() {
    vector<int> disc(V, -1);
    vector<int> low(V, -1);
    vector<bool> stackMember(V, false);
    stack<int> st;

    for (int i = 0; i < V; i++)
        if (disc[i] == -1)
            SCCUtil(i, disc, low, st, stackMember);
}

int main() {
    cout << "SCCs in first graph \n";
    Graph g1(5);
    g1.addEdge(1, 0);
    g1.addEdge(0, 2);
    g1.addEdge(2, 1);
    g1.addEdge(0, 3);
    g1.addEdge(3, 4);
    g1.SCC();
    return 0;
}
