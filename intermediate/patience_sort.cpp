/*
 * Problem Description:
 * Implement Patience Sort algorithm.
 * Patience sorting is a sorting algorithm inspired by the card game patience. 
 * A variant of the algorithm efficiently computes the length of a longest increasing subsequence in a given array.
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <queue>
using namespace std;

void patienceSort(vector<int>& arr) {
    vector<stack<int>> piles;
    
    for (int i = 0; i < arr.size(); i++) {
        int x = arr[i];
        bool found = false;
        for (int j = 0; j < piles.size(); j++) {
            if (piles[j].top() >= x) {
                piles[j].push(x);
                found = true;
                break;
            }
        }
        if (!found) {
            stack<int> newPile;
            newPile.push(x);
            piles.push_back(newPile);
        }
    }
    
    auto comp = [](pair<int, int> p1, pair<int, int> p2) {
        return p1.first > p2.first;
    };
    
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(comp)> pq(comp);
    
    for (int i = 0; i < piles.size(); i++) {
        pq.push({piles[i].top(), i});
        piles[i].pop();
    }
    
    int index = 0;
    while (!pq.empty()) {
        auto p = pq.top();
        pq.pop();
        arr[index++] = p.first;
        
        if (!piles[p.second].empty()) {
            pq.push({piles[p.second].top(), p.second});
            piles[p.second].pop();
        }
    }
}

int main() {
    vector<int> arr = {6, 3, 5, 1, 9, 7, 2, 8, 4};
    
    cout << "Original array: \n";
    for(int x: arr) cout << x << " ";
    cout << "\n";
    
    patienceSort(arr);
    
    cout << "Sorted array: \n";
    for(int x: arr) cout << x << " ";
    cout << "\n";
    return 0;
}
