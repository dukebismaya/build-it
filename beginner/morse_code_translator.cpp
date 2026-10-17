/*
 * Program Description:
 * Bidirectional Morse Code Translator in C++.
 * Converts standard English text to International Morse Code and Morse code back to text.
 *
 * Conventions:
 * - Letters within a word are separated by a space.
 * - Words are separated by a forward slash ' / '.
 * - Supports alphabetic letters (A-Z), numbers (0-9), and common punctuation.
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <cctype>

class MorseCodeTranslator {
    std::unordered_map<char, std::string> charToMorse;
    std::unordered_map<std::string, char> morseToChar;

    void initializeMaps() {
        const std::vector<std::pair<char, std::string>> table = {
            {'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
            {'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
            {'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
            {'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
            {'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
            {'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
            {'Y', "-.--"},  {'Z', "--.."},
            {'0', "-----"}, {'1', ".----"}, {'2', "..---"}, {'3', "...--"},
            {'4', "....-"}, {'5', "....."}, {'6', "-...."}, {'7', "--..."},
            {'8', "---.."}, {'9', "----."},
            {'.', ".-.-.-"},{',', "--..--"},{'?', "..--.."},{'!', "-.-.--"}
        };

        for (const auto& [ch, code] : table) {
            charToMorse[ch] = code;
            morseToChar[code] = ch;
        }
    }

public:
    MorseCodeTranslator() {
        initializeMaps();
    }

    std::string textToMorse(const std::string& text) {
        std::string morseResult = "";
        bool firstToken = true;

        for (size_t i = 0; i < text.length(); ++i) {
            char c = std::toupper(text[i]);

            if (c == ' ') {
                if (!firstToken) morseResult += " ";
                morseResult += "/";
                firstToken = false;
            } else if (charToMorse.find(c) != charToMorse.end()) {
                if (!firstToken) morseResult += " ";
                morseResult += charToMorse[c];
                firstToken = false;
            }
        }

        return morseResult;
    }

    std::string morseToText(const std::string& morse) {
        std::string textResult = "";
        std::stringstream ss(morse);
        std::string token;

        while (ss >> token) {
            if (token == "/") {
                textResult += " ";
            } else if (morseToChar.find(token) != morseToChar.end()) {
                textResult += morseToChar[token];
            } else {
                textResult += "?"; // Unknown symbol
            }
        }

        return textResult;
    }
};

int main() {
    std::cout << "==========================================\n";
    std::cout << "      Morse Code Translator (Bi-dir)      \n";
    std::cout << "==========================================\n\n";

    MorseCodeTranslator translator;
    std::string message = "SOS HELLO WORLD 2026";

    std::cout << "Original Text   : \"" << message << "\"\n";

    std::string encoded = translator.textToMorse(message);
    std::cout << "Morse Code      : " << encoded << "\n";

    std::string decoded = translator.morseToText(encoded);
    std::cout << "Decoded Back    : \"" << decoded << "\"\n";
    std::cout << "Integrity Check : " << (message == decoded ? "PASSED [100%]" : "FAILED") << "\n\n";

    std::cout << "--- Decoding Secret Signal ---\n";
    std::string secret = "... --- ... / -... ..- .. .-.. -.. / .. -";
    std::cout << "Signal   : " << secret << "\n";
    std::cout << "Decoded  : " << translator.morseToText(secret) << "\n";

    return 0;
}
