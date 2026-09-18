/*
 * Problem Description:
 * Implement a Segment Tree for Range Sum Query.
 * It allows finding the sum of elements in an array from index L to R in O(log n) time,
 * and also updating an element in O(log n) time.
 */

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class SegmentTree {
    vector<int> st;
    vector<int> arr;
    int n;

    void build(int node, int start, int end) {
        if (start == end) {
            st[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            build(2 * node, start, mid);
            build(2 * node + 1, mid + 1, end);
            st[node] = st[2 * node] + st[2 * node + 1];
        }
    }

    void update(int node, int start, int end, int idx, int val) {
        if (start == end) {
            arr[idx] += val;
            st[node] += val;
        } else {
            int mid = (start + end) / 2;
            if (start <= idx && idx <= mid) {
                update(2 * node, start, mid, idx, val);
            } else {
                update(2 * node + 1, mid + 1, end, idx, val);
            }
            st[node] = st[2 * node] + st[2 * node + 1];
        }
    }

    int query(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            return 0;
        }
        if (l <= start && end <= r) {
            return st[node];
        }
        int mid = (start + end) / 2;
        int p1 = query(2 * node, start, mid, l, r);
        int p2 = query(2 * node + 1, mid + 1, end, l, r);
        return p1 + p2;
    }

public:
    SegmentTree(vector<int>& input) {
        arr = input;
        n = arr.size();
        int x = (int)(ceil(log2(n))); 
        int max_size = 2 * (int)pow(2, x) - 1; 
        st.resize(max_size + 1);
        build(1, 0, n - 1);
    }

    void updateValue(int i, int new_val) {
        if (i < 0 || i > n - 1) {
            cout << "Invalid Input" << endl;
            return;
        }
        int diff = new_val - arr[i];
        update(1, 0, n - 1, i, diff);
    }

    int getSum(int l, int r) {
        if (l < 0 || r > n - 1 || l > r) {
            cout << "Invalid Input" << endl;
            return -1;
        }
        return query(1, 0, n - 1, l, r);
    }
};

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    SegmentTree tree(arr);

    cout << "Sum of values in given range (1, 3) = " << tree.getSum(1, 3) << endl;

    tree.updateValue(1, 10);
    cout << "Updated array[1] to 10" << endl;

    cout << "Sum of values in given range (1, 3) = " << tree.getSum(1, 3) << endl;

    return 0;
}
