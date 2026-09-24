/*
 * Problem Description:
 * Implement Strand Sort algorithm.
 * Strand sort is a sorting algorithm. It works by repeatedly pulling sorted sublists out of the 
 * list to be sorted and merging them with a result array.
 */

#include <iostream>
#include <list>
using namespace std;

void strandSort(list<int> &ip, list<int> &op) {
    if (ip.empty())
        return;
        
    list<int> sublist;
    sublist.push_back(ip.front());
    ip.pop_front();
    
    for (auto it = ip.begin(); it != ip.end(); ) {
        if (*it > sublist.back()) {
            sublist.push_back(*it);
            it = ip.erase(it);
        }
        else
            it++;
    }
    
    op.merge(sublist);
    strandSort(ip, op);
}

int main() {
    list<int> ip{10, 5, 30, 40, 2, 4, 9};
    list<int> op;
    
    strandSort(ip, op);
    
    cout << "Sorted array: \n";
    for (auto x : op)
        cout << x << " ";
    cout << endl;
    return 0;
}
