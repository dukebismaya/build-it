/*
 * Problem Description:
 * Implement Viterbi Algorithm conceptually.
 * The Viterbi algorithm is a dynamic programming algorithm for finding the most likely sequence of hidden states.
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;

void viterbi() {
    vector<string> states = {"Healthy", "Fever"};
    vector<string> observations = {"normal", "cold", "dizzy"};
    vector<double> start_p = {0.6, 0.4};
    
    // Transition probabilities
    vector<vector<double>> trans_p = {
        {0.7, 0.3}, // Healthy -> Healthy, Fever
        {0.4, 0.6}  // Fever -> Healthy, Fever
    };
    
    // Emission probabilities
    vector<vector<double>> emit_p = {
        {0.5, 0.4, 0.1}, // Healthy -> normal, cold, dizzy
        {0.1, 0.3, 0.6}  // Fever -> normal, cold, dizzy
    };
    
    // Sequence: normal, cold, dizzy -> indices: 0, 1, 2
    vector<int> obs = {0, 1, 2};
    
    int T = obs.size();
    int N = states.size();
    
    vector<vector<double>> V(N, vector<double>(T, 0));
    vector<vector<int>> path(N, vector<int>(T, 0));
    
    // Initialize
    for (int i = 0; i < N; i++) {
        V[i][0] = start_p[i] * emit_p[i][obs[0]];
        path[i][0] = i;
    }
    
    // Run Viterbi
    for (int t = 1; t < T; t++) {
        for (int j = 0; j < N; j++) {
            double max_p = -1;
            int max_i = -1;
            for (int i = 0; i < N; i++) {
                double p = V[i][t-1] * trans_p[i][j] * emit_p[j][obs[t]];
                if (p > max_p) {
                    max_p = p;
                    max_i = i;
                }
            }
            V[j][t] = max_p;
            path[j][t] = max_i;
        }
    }
    
    // Backtrack
    double max_p = -1;
    int max_i = -1;
    for (int i = 0; i < N; i++) {
        if (V[i][T-1] > max_p) {
            max_p = V[i][T-1];
            max_i = i;
        }
    }
    
    vector<int> best_path(T);
    best_path[T-1] = max_i;
    for (int t = T - 1; t > 0; t--) {
        best_path[t-1] = path[best_path[t]][t];
    }
    
    cout << "Most likely state sequence:\n";
    for (int i = 0; i < T; i++) {
        cout << states[best_path[i]] << " ";
    }
    cout << "\n";
}

int main() {
    viterbi();
    return 0;
}
