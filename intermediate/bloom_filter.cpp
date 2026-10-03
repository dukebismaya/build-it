/*
 * Problem Description:
 * Implement a Bloom Filter conceptually.
 * A Bloom filter is a space-efficient probabilistic data structure that is used 
 * to test whether an element is a member of a set.
 */

#include <iostream>
#include <vector>
#include <string>
using namespace std;

class BloomFilter {
private:
    vector<bool> bitArray;
    int size;

    // Simple hash function 1
    int hash1(string s) {
        int hash = 0;
        for (char c : s) {
            hash = (hash + c) % size;
        }
        return hash;
    }

    // Simple hash function 2
    int hash2(string s) {
        int hash = 1;
        for (char c : s) {
            hash = (hash * c) % size;
        }
        return hash;
    }

    // Simple hash function 3
    int hash3(string s) {
        int hash = 7;
        for (char c : s) {
            hash = (hash * 31 + c) % size;
        }
        return hash;
    }

public:
    BloomFilter(int s) {
        size = s;
        bitArray.resize(size, false);
    }

    void insert(string s) {
        bitArray[hash1(s)] = true;
        bitArray[hash2(s)] = true;
        bitArray[hash3(s)] = true;
    }

    bool lookup(string s) {
        return bitArray[hash1(s)] && bitArray[hash2(s)] && bitArray[hash3(s)];
    }
};

int main() {
    BloomFilter bf(100);

    bf.insert("apple");
    bf.insert("banana");

    cout << "Lookup 'apple': " << (bf.lookup("apple") ? "Probably present" : "Definitely not present") << endl;
    cout << "Lookup 'banana': " << (bf.lookup("banana") ? "Probably present" : "Definitely not present") << endl;
    cout << "Lookup 'grape': " << (bf.lookup("grape") ? "Probably present" : "Definitely not present") << endl;

    return 0;
}
