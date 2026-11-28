/*
 * ============================================================
 *            RFC 4648 Base64 and Base32 Binary Codec
 * ============================================================
 *
 * Description:
 *   Binary-to-text encodings represent arbitrary byte streams
 *   using printable ASCII characters so they survive JSON, HTTP
 *   headers, JWT tokens, email (MIME), and TOTP 2FA secrets:
 *
 *   1. Base64 (RFC 4648 Section 4):
 *      Groups 3 bytes (24 bits) into 4 6-bit symbols from
 *      `A-Z a-z 0-9 + /` with `=` padding when input length is
 *      not a multiple of 3 (4/3 = 33% size overhead).
 *
 *   2. Base32 (RFC 4648 Section 6):
 *      Groups 5 bytes (40 bits) into 8 5-bit symbols from the
 *      case-insensitive alphabet `A-Z 2-7` with `=` padding
 *      (8/5 = 60% overhead, used in TOTP authenticator keys and
 *      case-insensitive DNS/filenames).
 *
 * Complexity:
 *   Time:  O(N) for both encoding and decoding
 *   Space: O(N) for the output buffer
 * ============================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <random>

namespace rfc4648 {

    constexpr char B64_ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    constexpr char B32_ALPHABET[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

    std::string base64_encode(const uint8_t* data, size_t len) {
        std::string out;
        out.reserve(((len + 2) / 3) * 4);
        for (size_t i = 0; i < len; i += 3) {
            uint32_t v = static_cast<uint32_t>(data[i]) << 16;
            if (i + 1 < len) v |= static_cast<uint32_t>(data[i + 1]) << 8;
            if (i + 2 < len) v |= static_cast<uint32_t>(data[i + 2]);

            out.push_back(B64_ALPHABET[(v >> 18) & 0x3F]);
            out.push_back(B64_ALPHABET[(v >> 12) & 0x3F]);
            out.push_back((i + 1 < len) ? B64_ALPHABET[(v >> 6) & 0x3F] : '=');
            out.push_back((i + 2 < len) ? B64_ALPHABET[v & 0x3F] : '=');
        }
        return out;
    }

    bool base64_decode(const std::string& s, std::vector<uint8_t>& out) {
        static const std::array<int, 256> lut = [] {
            std::array<int, 256> t{};
            t.fill(-1);
            for (int i = 0; i < 64; ++i) t[static_cast<unsigned char>(B64_ALPHABET[i])] = i;
            return t;
        }();
        out.clear();
        if (s.size() % 4 != 0) return false;
        for (size_t i = 0; i < s.size(); i += 4) {
            int a = lut[static_cast<unsigned char>(s[i])];
            int b = lut[static_cast<unsigned char>(s[i + 1])];
            int c = (s[i + 2] == '=') ? 0 : lut[static_cast<unsigned char>(s[i + 2])];
            int d = (s[i + 3] == '=') ? 0 : lut[static_cast<unsigned char>(s[i + 3])];
            if (a < 0 || b < 0 || c < 0 || d < 0) return false;
            uint32_t v = (static_cast<uint32_t>(a) << 18) | (static_cast<uint32_t>(b) << 12) |
                         (static_cast<uint32_t>(c) << 6)  | static_cast<uint32_t>(d);
            out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
            if (s[i + 2] != '=') out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
            if (s[i + 3] != '=') out.push_back(static_cast<uint8_t>(v & 0xFF));
        }
        return true;
    }

    std::string base32_encode(const uint8_t* data, size_t len) {
        std::string out;
        out.reserve(((len + 4) / 5) * 8);
        for (size_t i = 0; i < len; i += 5) {
            uint64_t v = 0;
            size_t chunk = (len - i < 5) ? (len - i) : 5;
            for (size_t j = 0; j < 5; ++j)
                v = (v << 8) | (j < chunk ? data[i + j] : 0);

            int valid_chars = (chunk == 1) ? 2 : (chunk == 2) ? 4 : (chunk == 3) ? 5 : (chunk == 4) ? 7 : 8;
            for (int k = 0; k < 8; ++k) {
                if (k < valid_chars)
                    out.push_back(B32_ALPHABET[(v >> (35 - 5 * k)) & 0x1F]);
                else
                    out.push_back('=');
            }
        }
        return out;
    }

    bool base32_decode(const std::string& s, std::vector<uint8_t>& out) {
        static const std::array<int, 256> lut = [] {
            std::array<int, 256> t{};
            t.fill(-1);
            for (int i = 0; i < 32; ++i) t[static_cast<unsigned char>(B32_ALPHABET[i])] = i;
            return t;
        }();
        out.clear();
        if (s.size() % 8 != 0) return false;
        for (size_t i = 0; i < s.size(); i += 8) {
            uint64_t v = 0;
            int pad = 0;
            for (int k = 0; k < 8; ++k) {
                char ch = s[i + k];
                if (ch == '=') {
                    v <<= 5;
                    ++pad;
                } else {
                    int d = lut[static_cast<unsigned char>(ch)];
                    if (d < 0 || pad > 0) return false;
                    v = (v << 5) | static_cast<uint64_t>(d);
                }
            }
            int bytes = (pad == 6) ? 1 : (pad == 4) ? 2 : (pad == 3) ? 3 : (pad == 1) ? 4 : (pad == 0) ? 5 : -1;
            if (bytes < 0) return false;
            for (int b = 0; b < bytes; ++b)
                out.push_back(static_cast<uint8_t>((v >> (32 - 8 * b)) & 0xFF));
        }
        return true;
    }

} // namespace rfc4648

int main() {
    using namespace rfc4648;
    std::cout << "============================================================\n";
    std::cout << "            RFC 4648 Base64 and Base32 Binary Codec\n";
    std::cout << "============================================================\n\n";

    // Official RFC 4648 Section 10 test vectors: "", "f", "fo", "foo", "foob", "fooba", "foobar"
    struct Vec { const char* raw; const char* b64; const char* b32; };
    Vec rfc_vectors[] = {
        {"",       "",         ""},
        {"f",      "Zg==",     "MY======"},
        {"fo",     "Zm8=",     "MZXQ===="},
        {"foo",    "Zm9v",     "MZXW6==="},
        {"foob",   "Zm9vYg==", "MZXW6YQ="},
        {"fooba",  "Zm9vYmE=", "MZXW6YTB"},
        {"foobar", "Zm9vYmFy", "MZXW6YTBOI======"}
    };

    bool ok = true;
    std::cout << "RFC 4648 Section 10 Test Vectors:\n";
    for (const auto& tv : rfc_vectors) {
        std::string in(tv.raw);
        const auto* p = reinterpret_cast<const uint8_t*>(in.data());
        std::string e64 = base64_encode(p, in.size());
        std::string e32 = base32_encode(p, in.size());
        std::cout << "  \"" << in << "\" -> B64=\"" << e64 << "\", B32=\"" << e32 << "\"\n";
        if (e64 != tv.b64 || e32 != tv.b32) ok = false;
    }

    std::mt19937 rng(2026);
    for (int len = 0; len <= 300 && ok; ++len) {
        std::vector<uint8_t> raw(len), dec64, dec32;
        for (auto& b : raw) b = static_cast<uint8_t>(rng() & 0xFF);
        std::string s64 = base64_encode(raw.data(), raw.size());
        std::string s32 = base32_encode(raw.data(), raw.size());
        if (!base64_decode(s64, dec64) || dec64 != raw) ok = false;
        if (!base32_decode(s32, dec32) || dec32 != raw) ok = false;
    }

    std::cout << "\nRFC 4648 Section 10 vectors & random binary round-trips (0..300 B): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
