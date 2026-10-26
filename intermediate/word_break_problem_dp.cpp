/*
 * Program Description:
 * Word Break Problem using Dynamic Programming.
 * Given a string S and a dictionary of words, determine if S can be segmented
 * into a space-separated sequence of one or more dictionary words.
 *
 * DP State Definition:
 * dp[i] = true if prefix S[0..i-1] can be segmented into valid dictionary words.
 *
 * Transition:
 * dp[i] = true if there exists some j (0 <= j < i) such that:
 * dp[j] == true AND S[j..i-1] is present in the dictionary.
 *
 * Sentence Reconstruction:
 * parent[i] tracks the starting index j of the last word ending at index i,
 * allowing full sentence reconstruction with spaces.
 *
 * Time Complexity: O(N^2) where N is the length of the string.
 * Space Complexity: O(N + total dictionary characters).
 */

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

struct WordBreakResult {
    bool canBreak;
    std::string segmentedSentence;
};

WordBreakResult wordBreak(const std::string& s, const std::unordered_set<std::string>& dict) {
    int n = s.length();
    std::vector<bool> dp(n + 1, false);
    std::vector<int> parent(n + 1, -1);
    dp[0] = true; // Empty string base case

    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (dp[j]) {
                std::string word = s.substr(j, i - j);
                if (dict.find(word) != dict.end()) {
                    dp[i] = true;
                    parent[i] = j;
                    break;
                }
            }
        }
    }

    if (!dp[n]) {
        return {false, ""};
    }

    // Reconstruct segmented sentence
    std::vector<std::string> words;
    int curr = n;
    while (curr > 0) {
        int prev = parent[curr];
        words.push_back(s.substr(prev, curr - prev));
        curr = prev;
    }

    std::string sentence = "";
    for (int i = static_cast<int>(words.size()) - 1; i >= 0; --i) {
        sentence += words[i] + (i > 0 ? " " : "");
    }

    return {true, sentence};
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "        Word Break Problem (DP)           \n";
    std::cout << "==========================================\n\n";

    std::unordered_set<std::string> dict = {
        "apple", "pen", "applepen", "pine", "pineapple",
        "code", "build", "it", "fast", "cpp", "practice"
    };

    std::vector<std::string> testStrings = {
        "applepenapple",
        "pineapplepen",
        "builditpracticecppfast",
        "applepenxyz",
        "codebuild"
    };

    for (const auto& str : testStrings) {
        auto res = wordBreak(str, dict);
        std::cout << "Target String: \"" << str << "\"\n";
        if (res.canBreak) {
            std::cout << "  Segmentable : YES [PASS]\n";
            std::cout << "  Sentence    : \"" << res.segmentedSentence << "\"\n";
        } else {
            std::cout << "  Segmentable : NO [Cannot be segmented using dictionary]\n";
        }
        std::cout << "------------------------------------------\n";
    }

    return 0;
}
