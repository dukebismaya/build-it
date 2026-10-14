#include <iostream>
#include <vector>
#include <string>

// Function to generate all valid combinations of N pairs of parentheses
// Uses backtracking: at each step, we can add '(' if open < n, or ')' if close < open.
void generateParentheses(int n, int open, int close, std::string& current,
                         std::vector<std::string>& result) {
    if (static_cast<int>(current.size()) == 2 * n) {
        result.push_back(current);
        return;
    }

    if (open < n) {
        current.push_back('(');
        generateParentheses(n, open + 1, close, current, result);
        current.pop_back();
    }

    if (close < open) {
        current.push_back(')');
        generateParentheses(n, open, close + 1, current, result);
        current.pop_back();
    }
}

int main() {
    for (int n = 1; n <= 4; ++n) {
        std::vector<std::string> result;
        std::string current;
        generateParentheses(n, 0, 0, current, result);

        std::cout << "n = " << n << " (" << result.size() << " combinations):\n";
        for (const auto& s : result) {
            std::cout << "  " << s << "\n";
        }
        std::cout << "\n";
    }
    return 0;
}
