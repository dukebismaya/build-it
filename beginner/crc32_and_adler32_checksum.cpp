/*
 * ============================================================
 *             CRC-32 (IEEE 802.3) & Adler-32 Checksums
 * ============================================================
 *
 * Description:
 *   Fast error-detecting checksums used in networking, archives,
 *   and file formats (Ethernet, ZIP, PNG, gzip, zlib):
 *
 *   1. CRC-32 (IEEE 802.3 / ITU-T V.42):
 *      Treats the message as a polynomial over GF(2) and divides
 *      by the reflected generator polynomial 0xEDB88320
 *      (normal form 0x04C11DB7: x^32 + x^26 + x^23 + ... + 1).
 *      A 256-entry byte lookup table processes 8 bits per step:
 *        crc = (crc >> 8) ^ table[(crc ^ byte) & 0xFF]
 *      Standard check value for ASCII "123456789" is 0xCBF43926.
 *
 *   2. Adler-32 (Mark Adler, RFC 1950 / zlib):
 *      Maintains two 16-bit sums modulo prime 65521:
 *        A = (1 + sum d_i) mod 65521
 *        B = (sum_{i=1}^N A_i) mod 65521
 *        Adler32 = (B << 16) | A
 *      Standard check value for ASCII "123456789" is 0x091E01DE.
 *
 * Complexity:
 *   Time:  O(N) (1 table lookup + XOR per byte for CRC-32)
 *   Space: O(1) (1 KB static lookup table)
 * ============================================================
 */

#include <iostream>
#include <array>
#include <string>
#include <cstdint>
#include <iomanip>

namespace checksums {

    constexpr uint32_t CRC32_POLY_REFLECTED = 0xEDB88320u;

    const std::array<uint32_t, 256>& crc32_table() {
        static std::array<uint32_t, 256> tbl = [] {
            std::array<uint32_t, 256> t{};
            for (uint32_t i = 0; i < 256; ++i) {
                uint32_t c = i;
                for (int b = 0; b < 8; ++b)
                    c = (c & 1) ? (CRC32_POLY_REFLECTED ^ (c >> 1)) : (c >> 1);
                t[i] = c;
            }
            return t;
        }();
        return tbl;
    }

    uint32_t crc32_table_driven(const uint8_t* data, size_t len, uint32_t prev = 0) {
        const auto& tbl = crc32_table();
        uint32_t crc = ~prev;
        for (size_t i = 0; i < len; ++i)
            crc = (crc >> 8) ^ tbl[(crc ^ data[i]) & 0xFFu];
        return ~crc;
    }

    uint32_t crc32_bitwise(const uint8_t* data, size_t len) {
        uint32_t crc = 0xFFFFFFFFu;
        for (size_t i = 0; i < len; ++i) {
            crc ^= data[i];
            for (int b = 0; b < 8; ++b)
                crc = (crc & 1) ? (CRC32_POLY_REFLECTED ^ (crc >> 1)) : (crc >> 1);
        }
        return ~crc;
    }

    uint32_t adler32(const uint8_t* data, size_t len) {
        constexpr uint32_t MOD_ADLER = 65521u;
        uint32_t a = 1, b = 0;
        size_t i = 0;
        // Defer modulo for up to 5552 bytes (NMAX before 32-bit overflow)
        while (len > 0) {
            size_t tlen = len > 5552 ? 5552 : len;
            len -= tlen;
            do {
                a += data[i++];
                b += a;
            } while (--tlen);
            a %= MOD_ADLER;
            b %= MOD_ADLER;
        }
        return (b << 16) | a;
    }

} // namespace checksums

int main() {
    using namespace checksums;
    std::cout << "============================================================\n";
    std::cout << "             CRC-32 (IEEE 802.3) & Adler-32 Checksums\n";
    std::cout << "============================================================\n\n";

    std::string vec = "123456789";
    const auto* ptr = reinterpret_cast<const uint8_t*>(vec.data());
    uint32_t c_tbl = crc32_table_driven(ptr, vec.size());
    uint32_t c_bit = crc32_bitwise(ptr, vec.size());
    uint32_t adl   = adler32(ptr, vec.size());

    std::cout << "Standard test vector \"123456789\":\n";
    std::cout << "  CRC-32 (table)   = 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << c_tbl
              << " (expected 0xCBF43926)\n";
    std::cout << "  CRC-32 (bitwise) = 0x" << std::setw(8) << c_bit << "\n";
    std::cout << "  Adler-32         = 0x" << std::setw(8) << adl
              << " (expected 0x091E01DE)\n" << std::dec << std::setfill(' ');

    bool ok = (c_tbl == 0xCBF43926u && c_bit == 0xCBF43926u && adl == 0x091E01DEu);

    // Streaming chunked CRC-32 == one-shot CRC-32 == bitwise CRC-32, and all single-bit flips detected
    std::string payload = "The quick brown fox jumps over the lazy dog - IEEE 802.3 frame payload!";
    const auto* p = reinterpret_cast<const uint8_t*>(payload.data());
    uint32_t full_crc = crc32_table_driven(p, payload.size());
    if (full_crc != crc32_bitwise(p, payload.size())) ok = false;

    uint32_t stream_crc = crc32_table_driven(p, 19);
    stream_crc = crc32_table_driven(p + 19, payload.size() - 19, stream_crc);
    if (stream_crc != full_crc) ok = false;

    for (size_t i = 0; i < payload.size() && ok; ++i) {
        for (int b = 0; b < 8; ++b) {
            std::string corrupted = payload;
            corrupted[i] ^= static_cast<char>(1 << b);
            const auto* cp = reinterpret_cast<const uint8_t*>(corrupted.data());
            if (crc32_table_driven(cp, corrupted.size()) == full_crc) ok = false;
        }
    }

    std::cout << "\nIEEE 802.3 / RFC 1950 vectors, streaming & 100% single-bit flip detection: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
