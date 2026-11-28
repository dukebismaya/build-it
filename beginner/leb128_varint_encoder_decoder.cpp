/*
 * ============================================================
 *        LEB128 & ZigZag Variable-Length Integer Codec
 * ============================================================
 *
 * Description:
 *   Variable-length integer (varint) encodings store small
 *   integers in 1-2 bytes instead of fixed 8 bytes, forming the
 *   core binary format of Protocol Buffers, WebAssembly (Wasm),
 *   DWARF debugging tables, and Apache Avro/ORC:
 *
 *   1. Unsigned LEB128 (ULEB128):
 *      Splits a 64-bit unsigned integer into 7-bit groups from
 *      least significant to most significant. The top bit (0x80)
 *      of each byte is the continuation flag (1 = more bytes
 *      follow, 0 = final byte).
 *
 *   2. Signed LEB128 (SLEB128 - used in Wasm & DWARF):
 *      Two's-complement 7-bit chunks with sign-extension stop
 *      condition: final byte has continuation bit 0 and its
 *      6th bit (0x40) matches the remaining sign bits (all 0s
 *      for >= 0, all 1s for < 0).
 *
 *   3. ZigZag + ULEB128 (used in Protocol Buffers `sint64`):
 *      Bijectively maps signed integers to unsigned integers by
 *      interleaving negatives and positives:
 *        zigzag(n) = (n << 1) ^ (n >> 63)
 *        0 -> 0, -1 -> 1, 1 -> 2, -2 -> 3, 2 -> 4, ...
 *      so small negative integers encode in a single byte!
 *
 * Complexity:
 *   Time:  O(ceil(bit_width / 7)) <= 10 bytes for 64-bit ints.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <climits>
#include <random>
#include <iomanip>

namespace varint_codec {

    void encode_uleb128(uint64_t val, std::vector<uint8_t>& out) {
        do {
            uint8_t byte = static_cast<uint8_t>(val & 0x7Fu);
            val >>= 7;
            if (val != 0) byte |= 0x80u;
            out.push_back(byte);
        } while (val != 0);
    }

    uint64_t decode_uleb128(const uint8_t* data, size_t& offset) {
        uint64_t result = 0;
        int shift = 0;
        for (;;) {
            uint8_t byte = data[offset++];
            result |= static_cast<uint64_t>(byte & 0x7Fu) << shift;
            if ((byte & 0x80u) == 0) break;
            shift += 7;
        }
        return result;
    }

    void encode_sleb128(int64_t val, std::vector<uint8_t>& out) {
        bool more = true;
        while (more) {
            uint8_t byte = static_cast<uint8_t>(val & 0x7F);
            val >>= 7; // arithmetic shift on two's-complement int64_t
            bool sign_bit = (byte & 0x40u) != 0;
            if ((val == 0 && !sign_bit) || (val == -1 && sign_bit))
                more = false;
            else
                byte |= 0x80u;
            out.push_back(byte);
        }
    }

    int64_t decode_sleb128(const uint8_t* data, size_t& offset) {
        uint64_t result = 0;
        int shift = 0;
        uint8_t byte = 0;
        do {
            byte = data[offset++];
            result |= static_cast<uint64_t>(byte & 0x7Fu) << shift;
            shift += 7;
        } while (byte & 0x80u);

        if (shift < 64 && (byte & 0x40u))
            result |= (~0ULL << shift);
        return static_cast<int64_t>(result);
    }

    uint64_t zigzag_encode(int64_t n) {
        return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
    }

    int64_t zigzag_decode(uint64_t u) {
        return static_cast<int64_t>((u >> 1) ^ (~(u & 1ULL) + 1ULL));
    }

} // namespace varint_codec

int main() {
    using namespace varint_codec;
    std::cout << "============================================================\n";
    std::cout << "        LEB128 & ZigZag Variable-Length Integer Codec\n";
    std::cout << "============================================================\n\n";

    // DWARF specification classic examples: 624485 -> [E5 8E 26], -123456 -> [C0 BB 78]
    std::vector<uint8_t> u_buf, s_buf;
    encode_uleb128(624485ULL, u_buf);
    encode_sleb128(-123456LL, s_buf);

    std::cout << "ULEB128(624485)  = [ ";
    for (uint8_t b : u_buf) std::cout << "0x" << std::hex << std::uppercase << static_cast<int>(b) << " ";
    std::cout << "] (DWARF spec: 0xE5 0x8E 0x26)\n";

    std::cout << "SLEB128(-123456) = [ ";
    for (uint8_t b : s_buf) std::cout << "0x" << std::hex << std::uppercase << static_cast<int>(b) << " ";
    std::cout << "] (DWARF spec: 0xC0 0xBB 0x78)\n" << std::dec;

    bool ok = (u_buf == std::vector<uint8_t>{0xE5, 0x8E, 0x26}) &&
              (s_buf == std::vector<uint8_t>{0xC0, 0xBB, 0x78});

    std::vector<int64_t> edge_cases = {
        0, 1, -1, 63, -64, 64, -65, 127, 128, -128, -129,
        624485, -123456, INT32_MAX, INT32_MIN, INT64_MAX, INT64_MIN
    };
    std::mt19937_64 rng(2026);
    for (int i = 0; i < 50000; ++i) {
        int shift = rng() % 64;
        edge_cases.push_back(static_cast<int64_t>(rng()) >> shift);
    }

    std::vector<uint8_t> stream_u, stream_s, stream_z;
    for (int64_t v : edge_cases) {
        encode_uleb128(static_cast<uint64_t>(v), stream_u);
        encode_sleb128(v, stream_s);
        encode_uleb128(zigzag_encode(v), stream_z);
    }

    size_t off_u = 0, off_s = 0, off_z = 0;
    for (int64_t v : edge_cases) {
        uint64_t gu = decode_uleb128(stream_u.data(), off_u);
        int64_t  gs = decode_sleb128(stream_s.data(), off_s);
        int64_t  gz = zigzag_decode(decode_uleb128(stream_z.data(), off_z));
        if (gu != static_cast<uint64_t>(v) || gs != v || gz != v) { ok = false; break; }
    }
    if (off_u != stream_u.size() || off_s != stream_s.size() || off_z != stream_z.size())
        ok = false;

    std::cout << "\nEncoded 50,017 integers (raw = " << (edge_cases.size() * 8)
              << " B, ZigZag+ULEB128 = " << stream_z.size() << " B).\n";
    std::cout << "DWARF vectors & 64-bit boundary round-trip check: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
