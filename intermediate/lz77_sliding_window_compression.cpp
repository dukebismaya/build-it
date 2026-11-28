/*
 * ============================================================
 *         LZ77 Sliding-Window Lossless Data Compression
 * ============================================================
 *
 * Description:
 *   Ziv & Lempel's 1977 algorithm (LZ77) is the dictionary
 *   compression engine behind DEFLATE, gzip, ZIP, PNG, and Zstandard.
 *
 *   Instead of building an explicit dictionary table, LZ77 uses
 *   the last W bytes of already-seen text as a sliding search
 *   window and matches the longest prefix of the lookahead buffer:
 *     - Each token is a triple (offset, length, next_char) where:
 *         * offset = distance backward into the sliding window
 *         * length = number of matched bytes (may exceed offset
 *           for periodic run-length patterns like "abababab"!)
 *         * next_char = first unmatched byte following the match
 *     - Hash-chain acceleration indexes 3-byte prefixes in the
 *       window so longest-match search inspects only positions
 *       sharing the same 3-byte hash instead of scanning all W bytes.
 *
 * Complexity:
 *   Compression:   O(N * max_chain_steps) time, O(W) space
 *   Decompression: O(N) time (simple byte copy loop!)
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <algorithm>

namespace lz77 {

    struct Token {
        uint16_t offset;
        uint16_t length;
        char next;
        bool has_next;
    };

    class LZ77Compressor {
        static constexpr int WINDOW_SIZE = 4096;
        static constexpr int MAX_MATCH = 255;
        static constexpr int MIN_MATCH = 3;
        static constexpr int HASH_SIZE = 1 << 14;
        static constexpr int MAX_CHAIN = 64;

        static int hash3(const std::string& s, int pos) {
            uint32_t h = (static_cast<uint8_t>(s[pos]) << 16) |
                         (static_cast<uint8_t>(s[pos + 1]) << 8) |
                          static_cast<uint8_t>(s[pos + 2]);
            return static_cast<int>((h * 2654435761u) >> (32 - 14));
        }

    public:
        static std::vector<Token> compress(const std::string& input) {
            int n = static_cast<int>(input.size());
            std::vector<Token> out;
            std::vector<int> head(HASH_SIZE, -1), prev(n, -1);

            auto insert_pos = [&](int p) {
                if (p + 2 < n) {
                    int h = hash3(input, p);
                    prev[p] = head[h];
                    head[h] = p;
                }
            };

            int pos = 0;
            while (pos < n) {
                int best_len = 0, best_off = 0;
                int max_len = std::min(MAX_MATCH, n - pos);

                if (max_len >= MIN_MATCH) {
                    int h = hash3(input, pos);
                    int cand = head[h];
                    int steps = 0;
                    int min_pos = std::max(0, pos - WINDOW_SIZE);
                    while (cand >= min_pos && steps < MAX_CHAIN) {
                        int len = 0;
                        while (len < max_len && input[cand + len] == input[pos + len])
                            ++len;
                        if (len >= MIN_MATCH && len > best_len) {
                            best_len = len;
                            best_off = pos - cand;
                            if (len == max_len) break;
                        }
                        cand = prev[cand];
                        ++steps;
                    }
                }

                for (int i = 0; i <= best_len && pos + i < n; ++i)
                    insert_pos(pos + i);

                pos += best_len;
                if (pos < n) {
                    out.push_back({static_cast<uint16_t>(best_off),
                                   static_cast<uint16_t>(best_len),
                                   input[pos], true});
                    ++pos;
                } else {
                    out.push_back({static_cast<uint16_t>(best_off),
                                   static_cast<uint16_t>(best_len),
                                   '\0', false});
                }
            }
            return out;
        }

        static std::string decompress(const std::vector<Token>& tokens) {
            std::string out;
            for (const auto& t : tokens) {
                if (t.length > 0) {
                    size_t start = out.size() - t.offset;
                    // Copy byte-by-byte to support overlapping RLE matches (length > offset)
                    for (uint16_t i = 0; i < t.length; ++i)
                        out.push_back(out[start + i]);
                }
                if (t.has_next) out.push_back(t.next);
            }
            return out;
        }
    };

} // namespace lz77

int main() {
    using lz77::LZ77Compressor;
    std::cout << "============================================================\n";
    std::cout << "         LZ77 Sliding-Window Lossless Data Compression\n";
    std::cout << "============================================================\n\n";

    std::string demo = "abracadabra_abracadabra_abracadabra_aaaaaaaaaaaaaaa!";
    auto toks = LZ77Compressor::compress(demo);
    std::string rec = LZ77Compressor::decompress(toks);

    std::cout << "Input (" << demo.size() << " chars): \"" << demo << "\"\n";
    std::cout << "LZ77 stream (" << toks.size() << " tokens):\n  ";
    for (const auto& t : toks) {
        std::cout << "(" << t.offset << "," << t.length << ",";
        if (t.has_next) std::cout << "'" << t.next << "') ";
        else            std::cout << "EOF) ";
    }
    std::cout << "\n";

    // Structured log / source code benchmark
    std::string corpus;
    for (int i = 0; i < 400; ++i) {
        corpus += "[2026-11-28T12:00:00Z] INFO http.server status=200 path=/api/v1/users id="
                + std::to_string(i % 25) + " latency_ms=12\n";
    }
    auto c_toks = LZ77Compressor::compress(corpus);
    std::string c_rec = LZ77Compressor::decompress(c_toks);
    size_t raw_bytes = corpus.size();
    size_t comp_bytes = c_toks.size() * 4; // 12-bit offset + 8-bit length + 8-bit char = ~4 bytes/token

    std::cout << "\nServer log corpus: raw = " << raw_bytes << " bytes -> "
              << c_toks.size() << " tokens (~" << comp_bytes << " bytes, "
              << std::fixed << std::setprecision(2) << (static_cast<double>(raw_bytes) / comp_bytes)
              << "x compression ratio)\n";

    bool ok = (rec == demo) && (c_rec == corpus) && (comp_bytes * 3 < raw_bytes);
    std::cout << "Lossless decompression & >3x log compression check: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
