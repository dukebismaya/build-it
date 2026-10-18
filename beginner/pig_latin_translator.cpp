/*
 * Program Description:
 * Pig Latin Sentence Translator in C++.
 * Translates English text to Pig Latin according to classic linguistic rules:
 *
 * Rules:
 * 1. If a word begins with a vowel sound (A, E, I, O, U), append "way" to the end:
 *    "apple" -> "appleway", "egg" -> "eggway".
 * 2. If a word begins with a consonant or consonant cluster, move all letters before
 *    the first vowel to the end and append "ay":
 *    "string" -> "ingstray", "hello" -> "ellohay".
 * 3. Preserves punctuation, whitespace, and capitalized first letters.
 */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

bool isVowel(char c) {
    c = std::tolower(c);
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

// Translates a single word to Pig Latin while preserving case and attached punctuation
std::string translateWord(const std::string& word) {
    if (word.empty()) return "";

    // Separate leading/trailing punctuation
    size_t start = 0;
    while (start < word.length() && !std::isalpha(word[start])) start++;

    size_t end = word.length();
    while (end > start && !std::isalpha(word[end - 1])) end--;

    if (start >= end) return word; // No alphabetic letters

    std::string leadingPunct = word.substr(0, start);
    std::string core = word.substr(start, end - start);
    std::string trailingPunct = word.substr(end);

    bool isCapitalized = std::isupper(core[0]);
    std::string lowerCore = core;
    for (char& c : lowerCore) c = std::tolower(c);

    std::string pigWord = "";

    if (isVowel(lowerCore[0])) {
        pigWord = lowerCore + "way";
    } else {
        // Find first vowel index
        size_t firstVowel = 0;
        while (firstVowel < lowerCore.length() && !isVowel(lowerCore[firstVowel])) {
            firstVowel++;
        }

        if (firstVowel == lowerCore.length()) {
            // No vowels (e.g., "my", "rhythm")
            pigWord = lowerCore + "ay";
        } else {
            pigWord = lowerCore.substr(firstVowel) + lowerCore.substr(0, firstVowel) + "ay";
        }
    }

    if (isCapitalized && !pigWord.empty()) {
        pigWord[0] = std::toupper(pigWord[0]);
    }

    return leadingPunct + pigWord + trailingPunct;
}

// Translates a full sentence containing spaces and punctuation
std::string translateSentence(const std::string& sentence) {
    std::stringstream ss(sentence);
    std::string word;
    std::string result = "";
    bool first = true;

    while (ss >> word) {
        if (!first) result += " ";
        result += translateWord(word);
        first = false;
    }

    return result;
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "           Pig Latin Translator           \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> sentences = {
        "Hello World!",
        "The quick brown fox jumps over the lazy dog.",
        "Eat apples and oranges every day.",
        "Simple C++ algorithms are fun and fast!"
    };

    for (const auto& s : sentences) {
        std::cout << "English   : " << s << "\n";
        std::cout << "Pig Latin : " << translateSentence(s) << "\n";
        std::cout << "--------------------------------------------------------\n";
    }

    return 0;
}
