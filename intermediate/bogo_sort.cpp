/*
 * Problem Description:
 * Implement Bogo Sort algorithm.
 * BogoSort also known as permutation sort, stupid sort, slow sort, shotgun sort or monkey sort 
 * is a particularly ineffective algorithm based on generate and test paradigm. 
 * The algorithm successively generates permutations of its input until it finds one that is sorted.
 */

#include <bits/stdc++.h>
using namespace std;

bool isSorted(int a[], int n) {
    while (--n >= 1)
        if (a[n] < a[n - 1])
            return false;
    return true;
}

void shuffle(int a[], int n) {
    for (int i = 0; i < n; i++)
        swap(a[i], a[rand() % n]);
}

void bogoSort(int a[], int n) {
    while (!isSorted(a, n))
        shuffle(a, n);
}

int main() {
    int a[] = {3, 2, 5, 1, 0, 4};
    int n = sizeof a / sizeof a[0];
    
    bogoSort(a, n);
    
    cout << "Sorted array :\n";
    for (int i = 0; i < n; i++)
        cout << a[i] << " ";
    cout << "\n";
    
    return 0;
}
