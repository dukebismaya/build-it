/*
 * Program Description:
 * Run-Length Encoding (RLE) is a basic lossless data compression algorithm.
 * Consecutive identical data elements (characters) are stored as a single value
 * and count (e.g., "AAAABBBCCDAA" -> "4A3B2C1D2A").
 *
 * This implementation provides:
 * 1. String Compression (encode)
 * 2. String Decompression (decode)
 * 3. Compression ratio computation
 * 4. Comprehensive test cases including edge cases (empty, single chars, no repeats).
 */

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip>

// Encodes a string into its Run-Length Encoded representation
std::string runLengthEncode(const std::string& input) {
    if (input.empty()) return "";

    std::string encoded = "";
    int n = input.length();

    for (int i = 0; i < n; ++i) {
        int count = 1;
        while (i + 1 < n && input[i] == input[i + 1]) {
            count++;
            i++;
        }
        encoded += std::to_string(count) + input[i];
    }

    return encoded;
}

// Decodes a Run-Length Encoded string back to original
std::string runLengthDecode(const std::string& encoded) {
    std::string decoded = "";
    int n = encoded.length();
    int i = 0;

    while (i < n) {
        int count = 0;
        // Parse the count (might be multi-digit)
        while (i < n && std::isdigit(encoded[i])) {
            count = count * 10 + (encoded[i] - '0');
            i++;
        }

        if (i < n) {
            char ch = encoded[i];
            decoded.append(count, ch);
            i++;
        }
    }

    return decoded;
}

void testRLE(const std::string& original) {
    std::string encoded = runLengthEncode(original);
    std::string decoded = runLengthDecode(encoded);

    double ratio = original.empty() ? 0.0 :
                   (1.0 - static_cast<double>(encoded.length()) / original.length()) * 100.0;

    std::cout << "Original   : \"" << original << "\" (" << original.length() << " chars)\n";
    std::cout << "Encoded    : \"" << encoded << "\" (" << encoded.length() << " chars)\n";
    std::cout << "Decoded    : \"" << decoded << "\"\n";
    std::cout << "Match?     : " << (original == decoded ? "YES [PASS]" : "NO [FAIL]") << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Savings    : " << ratio << "%\n";
    std::cout << "--------------------------------------------------------\n";
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Run-Length Encoding / Decoding      \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> testCases = {
        "WWWWWWWWWWWWBWWWWWWWWWWWWBBBWWWWWWWWWWWWWWWWWWWWWWWWB",
        "AABBBCCCCCDDDDDDEEEEEEE",
        "ABCDEF",
        "AAAAAAAAAA",
        "A",
        ""
    };

    for (const auto& test : testCases) {
        testRLE(test);
    }

    return 0;
}
