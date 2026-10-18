/*
 * Program Description:
 * Suffix Automaton (Directed Acyclic Word Graph - DAWG) in C++.
 * A powerful data structure representing all substrings of a given string
 * in linear time O(N) and linear memory O(N).
 *
 * Properties for a string of length N:
 * - Number of states (vertices): at most 2N - 1.
 * - Number of transitions (edges): at most 3N - 4.
 *
 * Capabilities implemented:
 * 1. Online linear time construction (O(N) total time).
 * 2. Substring query: checks whether pattern P is a substring in O(|P|) time.
 * 3. Total number of distinct substrings in O(states).
 * 4. Longest common substring (LCS) between two strings in O(|S1| + |S2|).
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>

struct State {
    int len;
    int link;
    std::map<char, int> next;
    long long paths = 0; // Number of distinct paths from this state
};

class SuffixAutomaton {
    std::vector<State> st;
    int sz, last;

public:
    SuffixAutomaton() {
        st.emplace_back();
        st[0].len = 0;
        st[0].link = -1;
        sz = 1;
        last = 0;
    }

    void extend(char c) {
        int cur = sz++;
        st.emplace_back();
        st[cur].len = st[last].len + 1;

        int p = last;
        while (p != -1 && !st[p].next.count(c)) {
            st[p].next[c] = cur;
            p = st[p].link;
        }

        if (p == -1) {
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if (st[p].len + 1 == st[q].len) {
                st[cur].link = q;
            } else {
                int clone = sz++;
                st.push_back(st[q]);
                st[clone].len = st[p].len + 1;

                while (p != -1 && st[p].next[c] == q) {
                    st[p].next[c] = clone;
                    p = st[p].link;
                }

                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
    }

    void build(const std::string& s) {
        for (char c : s) {
            extend(c);
        }
    }

    bool containsSubstring(const std::string& pattern) const {
        int curr = 0;
        for (char c : pattern) {
            if (!st[curr].next.count(c)) {
                return false;
            }
            curr = st[curr].next.at(c);
        }
        return true;
    }

    long long countDistinctSubstrings() {
        // Topological DP to count all unique paths from root
        std::vector<long long> dp(sz, 0);

        auto dfs = [&](auto& self, int u) -> long long {
            if (dp[u] != 0) return dp[u];
            long long count = 1; // Count empty extension from state u
            for (const auto& [ch, v] : st[u].next) {
                count += self(self, v);
            }
            return dp[u] = count;
        };

        return dfs(dfs, 0) - 1; // Subtract 1 for the empty string
    }

    // Finds the longest common substring with another string T
    std::string longestCommonSubstring(const std::string& t) const {
        int curr = 0, l = 0, best = 0, bestPos = 0;

        for (int i = 0; i < static_cast<int>(t.length()); ++i) {
            char c = t[i];
            while (curr != 0 && !st[curr].next.count(c)) {
                curr = st[curr].link;
                l = st[curr].len;
            }
            if (st[curr].next.count(c)) {
                curr = st[curr].next.at(c);
                l++;
            }
            if (l > best) {
                best = l;
                bestPos = i;
            }
        }

        return t.substr(bestPos - best + 1, best);
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Suffix Automaton (DAWG) Demo        \n";
    std::cout << "==========================================\n\n";

    std::string text = "abracadabra";
    std::cout << "Target Text: \"" << text << "\"\n\n";

    SuffixAutomaton sam;
    sam.build(text);

    // Substring queries
    std::vector<std::string> testPatterns = {"abra", "cada", "cadabra", "dabr", "xyz", "abracadabraz"};
    std::cout << "--- Substring Membership Queries (O(|Pattern|)) ---\n";
    for (const auto& pat : testPatterns) {
        bool exists = sam.containsSubstring(pat);
        std::cout << "  \"" << pat << "\": " << (exists ? "FOUND [YES]" : "NOT FOUND [NO]") << "\n";
    }

    // Count distinct substrings
    long long distinct = sam.countDistinctSubstrings();
    std::cout << "\nTotal Distinct Substrings of \"" << text << "\": " << distinct << "\n";

    // Longest Common Substring (LCS)
    std::string other = "cadabramagic";
    std::string lcs = sam.longestCommonSubstring(other);
    std::cout << "\nLongest Common Substring with \"" << other << "\": \"" << lcs
              << "\" (length " << lcs.length() << ")\n";

    return 0;
}
