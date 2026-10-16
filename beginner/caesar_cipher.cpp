/*
 * Program Description:
 * Caesar Cipher Encryption, Decryption, and Brute-Force Cracker.
 * One of the simplest and earliest known substitution ciphers.
 * Each letter in the plaintext is shifted by a fixed number of positions down the alphabet.
 *
 * Features:
 * 1. Encryption with arbitrary integer shift (handles large and negative shifts).
 * 2. Decryption using the inverse key.
 * 3. Preserves uppercase, lowercase, punctuation, and whitespace.
 * 4. Brute-force cracker displaying all 25 possible shift candidate plaintexts.
 */

#include <iostream>
#include <string>
#include <cctype>
#include <vector>

// Encrypts text using Caesar cipher with given shift
std::string caesarEncrypt(const std::string& text, int shift) {
    std::string result = text;
    // Normalize shift to [0, 25]
    shift = (shift % 26 + 26) % 26;

    for (char& c : result) {
        if (std::isupper(c)) {
            c = static_cast<char>('A' + (c - 'A' + shift) % 26);
        } else if (std::islower(c)) {
            c = static_cast<char>('a' + (c - 'a' + shift) % 26);
        }
    }
    return result;
}

// Decrypts text by applying the inverse shift
std::string caesarDecrypt(const std::string& cipherText, int shift) {
    return caesarEncrypt(cipherText, -shift);
}

// Cracks Caesar cipher by attempting all 25 possible shift keys
void bruteForceCrack(const std::string& cipherText) {
    std::cout << "Attempting all 25 shift keys for: \"" << cipherText << "\"\n";
    for (int key = 1; key <= 25; ++key) {
        std::string decrypted = caesarDecrypt(cipherText, key);
        std::cout << "  Shift " << (key < 10 ? " " : "") << key << ": " << decrypted << "\n";
    }
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Caesar Cipher & Brute-Force Cracker   \n";
    std::cout << "==========================================\n\n";

    std::string message = "The quick brown fox jumps over the lazy dog! (2026)";
    int key = 7;

    std::cout << "Original Message : " << message << "\n";
    std::cout << "Shift Key        : " << key << "\n\n";

    std::string encrypted = caesarEncrypt(message, key);
    std::cout << "Encrypted        : " << encrypted << "\n";

    std::string decrypted = caesarDecrypt(encrypted, key);
    std::cout << "Decrypted        : " << decrypted << "\n";
    std::cout << "Integrity Check  : " << (message == decrypted ? "PASSED" : "FAILED") << "\n\n";

    std::cout << "--- Brute Force Attack Demo ---\n";
    std::string secret = "Khoor, Zruog!";
    bruteForceCrack(secret);

    return 0;
}
