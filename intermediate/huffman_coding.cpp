/*
 * Program Description:
 * Huffman Coding Algorithm for Lossless Data Compression.
 * Assigns variable-length prefix-free codes to characters based on their frequencies.
 * More frequent characters get shorter codes; less frequent get longer codes.
 *
 * Components:
 * 1. Huffman Tree node & Min-Priority Queue
 * 2. Tree construction & Code generation
 * 3. Text encoding (plain text to bitstring)
 * 4. Text decoding (bitstring back to original text using tree traversal)
 * 5. Compression ratio and space efficiency statistics.
 *
 * Time Complexity: O(N log K) where N is text length, K is distinct character count.
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <iomanip>

struct HuffmanNode {
    char data;
    unsigned freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char d, unsigned f) : data(d), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(unsigned f, HuffmanNode* l, HuffmanNode* r) : data('\0'), freq(f), left(l), right(r) {}

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

// Comparator for priority queue (min-heap based on frequency)
struct CompareNode {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

class HuffmanCompressor {
    HuffmanNode* root = nullptr;
    std::unordered_map<char, std::string> huffmanCodes;

    void generateCodes(HuffmanNode* node, const std::string& code) {
        if (!node) return;

        if (node->isLeaf()) {
            huffmanCodes[node->data] = code.empty() ? "0" : code;
            return;
        }

        generateCodes(node->left, code + "0");
        generateCodes(node->right, code + "1");
    }

    void freeTree(HuffmanNode* node) {
        if (!node) return;
        freeTree(node->left);
        freeTree(node->right);
        delete node;
    }

public:
    HuffmanCompressor() = default;
    ~HuffmanCompressor() {
        freeTree(root);
    }

    // Builds the Huffman tree from input text
    void build(const std::string& text) {
        if (text.empty()) return;

        std::unordered_map<char, unsigned> freqMap;
        for (char ch : text) {
            freqMap[ch]++;
        }

        std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNode> minHeap;
        for (auto pair : freqMap) {
            minHeap.push(new HuffmanNode(pair.first, pair.second));
        }

        while (minHeap.size() > 1) {
            HuffmanNode* left = minHeap.top(); minHeap.pop();
            HuffmanNode* right = minHeap.top(); minHeap.pop();

            HuffmanNode* parent = new HuffmanNode(left->freq + right->freq, left, right);
            minHeap.push(parent);
        }

        root = minHeap.top();
        huffmanCodes.clear();
        generateCodes(root, "");
    }

    // Encodes string into bit sequence
    std::string encode(const std::string& text) const {
        std::string encoded = "";
        for (char ch : text) {
            encoded += huffmanCodes.at(ch);
        }
        return encoded;
    }

    // Decodes bit sequence back into original string
    std::string decode(const std::string& encoded) const {
        std::string decoded = "";
        if (!root) return decoded;

        if (root->isLeaf()) {
            return std::string(encoded.length(), root->data);
        }

        HuffmanNode* curr = root;
        for (char bit : encoded) {
            if (bit == '0') {
                curr = curr->left;
            } else {
                curr = curr->right;
            }

            if (curr->isLeaf()) {
                decoded += curr->data;
                curr = root;
            }
        }
        return decoded;
    }

    const std::unordered_map<char, std::string>& getCodes() const {
        return huffmanCodes;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Huffman Coding Compression Demo     \n";
    std::cout << "==========================================\n\n";

    std::string text = "build it practice keeps cpp sharp and fast";
    std::cout << "Original Text: \"" << text << "\"\n\n";

    HuffmanCompressor compressor;
    compressor.build(text);

    std::cout << "--- Generated Huffman Prefix Codes ---\n";
    for (const auto& [ch, code] : compressor.getCodes()) {
        if (ch == ' ') {
            std::cout << "' ' (space) : " << code << "\n";
        } else {
            std::cout << "'" << ch << "'         : " << code << "\n";
        }
    }

    std::string encoded = compressor.encode(text);
    std::string decoded = compressor.decode(encoded);

    size_t originalBits = text.length() * 8;
    size_t compressedBits = encoded.length();
    double ratio = (1.0 - static_cast<double>(compressedBits) / originalBits) * 100.0;

    std::cout << "\n--- Compression Results ---\n";
    std::cout << "Encoded Bitstring : " << encoded << "\n";
    std::cout << "Decoded Text      : \"" << decoded << "\"\n";
    std::cout << "Integrity Check   : " << (text == decoded ? "PERFECT [PASS]" : "FAILED") << "\n";
    std::cout << "Original Size     : " << originalBits << " bits (" << text.length() << " bytes)\n";
    std::cout << "Compressed Size   : " << compressedBits << " bits (" << (compressedBits + 7) / 8 << " bytes)\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Bit Space Savings : " << ratio << "%\n";

    return 0;
}
