/*
 * Problem Description:
 * Implement Sleep Sort algorithm using threads.
 * Sleep sort works by creating a separate thread for each element to be sorted.
 * Each thread sleeps for a duration proportional to the element's value.
 */

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

using namespace std;

mutex mtx;
vector<int> sorted_arr;

void sleepAndPush(int val) {
    this_thread::sleep_for(chrono::milliseconds(val * 10)); // Scale for visibility
    lock_guard<mutex> lock(mtx);
    sorted_arr.push_back(val);
}

void sleepSort(const vector<int>& arr) {
    vector<thread> threads;
    
    for (int val : arr) {
        threads.emplace_back(sleepAndPush, val);
    }
    
    for (auto& th : threads) {
        th.join();
    }
}

int main() {
    vector<int> arr = {5, 2, 8, 1, 9, 3};
    
    cout << "Original array: \n";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    
    sleepSort(arr);
    
    cout << "Sorted array: \n";
    for (int x : sorted_arr) cout << x << " ";
    cout << "\n";
    
    return 0;
}
