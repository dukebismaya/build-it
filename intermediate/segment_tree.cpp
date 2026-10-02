/*
 * Problem Description:
 * Implement a Segment Tree for Range Sum Queries.
 * Segment Tree is a basically a binary tree used for storing the intervals or segments. 
 * It allows querying which of the stored segments contain a given point.
 */

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

class SegmentTree {
    vector<int> st;
    vector<int> arr;
    int n;

    int getMid(int s, int e) { return s + (e - s) / 2; }

    int constructSTUtil(int ss, int se, int si) {
        if (ss == se) {
            st[si] = arr[ss];
            return arr[ss];
        }
        int mid = getMid(ss, se);
        st[si] = constructSTUtil(ss, mid, si * 2 + 1) +
                 constructSTUtil(mid + 1, se, si * 2 + 2);
        return st[si];
    }

    int getSumUtil(int ss, int se, int qs, int qe, int si) {
        if (qs <= ss && qe >= se)
            return st[si];
        if (se < qs || ss > qe)
            return 0;
        int mid = getMid(ss, se);
        return getSumUtil(ss, mid, qs, qe, 2 * si + 1) +
               getSumUtil(mid + 1, se, qs, qe, 2 * si + 2);
    }

    void updateValueUtil(int ss, int se, int i, int diff, int si) {
        if (i < ss || i > se)
            return;
        st[si] = st[si] + diff;
        if (se != ss) {
            int mid = getMid(ss, se);
            updateValueUtil(ss, mid, i, diff, 2 * si + 1);
            updateValueUtil(mid + 1, se, i, diff, 2 * si + 2);
        }
    }

public:
    SegmentTree(vector<int> inputArr) {
        arr = inputArr;
        n = arr.size();
        int x = (int)(ceil(log2(n)));
        int max_size = 2 * (int)pow(2, x) - 1;
        st.resize(max_size);
        constructSTUtil(0, n - 1, 0);
    }

    int getSum(int qs, int qe) {
        if (qs < 0 || qe > n - 1 || qs > qe) {
            cout << "Invalid Input";
            return -1;
        }
        return getSumUtil(0, n - 1, qs, qe, 0);
    }

    void updateValue(int i, int new_val) {
        if (i < 0 || i > n - 1) {
            cout << "Invalid Input";
            return;
        }
        int diff = new_val - arr[i];
        arr[i] = new_val;
        updateValueUtil(0, n - 1, i, diff, 0);
    }
};

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    SegmentTree tree(arr);
    
    cout << "Sum of values in given range = " << tree.getSum(1, 3) << endl;
    
    tree.updateValue(1, 10);
    
    cout << "Updated sum of values in given range = " << tree.getSum(1, 3) << endl;
    
    return 0;
}
