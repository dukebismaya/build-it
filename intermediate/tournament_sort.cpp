/*
 * Problem Description:
 * Implement Tournament Sort algorithm.
 * Tournament sort is a sorting algorithm that works by building a tournament tree (a heap-like structure).
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void tournamentSort(vector<int>& arr) {
    int n = arr.size();
    if (n <= 1) return;
    
    vector<int> sorted_arr;
    
    while (!arr.empty()) {
        vector<int> winners;
        vector<int> losers;
        
        while (arr.size() > 1) {
            int match1 = arr.back();
            arr.pop_back();
            int match2 = arr.back();
            arr.pop_back();
            
            if (match1 < match2) {
                winners.push_back(match1);
                losers.push_back(match2);
            } else {
                winners.push_back(match2);
                losers.push_back(match1);
            }
        }
        
        if (arr.size() == 1) {
            winners.push_back(arr.back());
            arr.pop_back();
        }
        
        sorted_arr.push_back(winners.front());
        winners.erase(winners.begin());
        
        for (int x : winners) arr.push_back(x);
        for (int x : losers) arr.push_back(x);
    }
    
    arr = sorted_arr;
}

int main() {
    vector<int> arr = {4, 3, 2, 6, 8, 1, 9, 5, 7};
    cout << "Original Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    tournamentSort(arr);
    
    cout << "Sorted Array: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    return 0;
}
