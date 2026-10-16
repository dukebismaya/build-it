/*
 * Program Description:
 * Luhn Algorithm (Mod 10 Formula) Validator and Check-Digit Generator.
 * Used worldwide to validate identification numbers, credit card numbers,
 * IMEI numbers, Canadian Social Insurance numbers, etc.
 *
 * Algorithm details:
 * 1. From the rightmost digit (excluding check digit), moving left, double every second digit.
 * 2. If doubling a digit results in a number > 9, sum the digits (or subtract 9).
 * 3. Take the sum of all digits.
 * 4. If the total modulo 10 is equal to 0, the number is valid.
 */

#include <iostream>
#include <string>
#include <vector>
#include <cctype>

// Cleans string, stripping whitespace and hyphens
std::string sanitizeNumber(const std::string& input) {
    std::string sanitized = "";
    for (char c : input) {
        if (std::isdigit(c)) {
            sanitized += c;
        }
    }
    return sanitized;
}

// Validates a complete number containing a Luhn check digit
bool isValidLuhn(const std::string& rawNumber) {
    std::string num = sanitizeNumber(rawNumber);
    if (num.length() <= 1) return false;

    int sum = 0;
    bool doubleDigit = false;

    for (int i = static_cast<int>(num.length()) - 1; i >= 0; --i) {
        int digit = num[i] - '0';

        if (doubleDigit) {
            digit *= 2;
            if (digit > 9) {
                digit -= 9;
            }
        }

        sum += digit;
        doubleDigit = !doubleDigit;
    }

    return (sum % 10 == 0);
}

// Calculates the required check digit to append to a partial number payload
int generateCheckDigit(const std::string& payload) {
    std::string num = sanitizeNumber(payload);
    int sum = 0;
    bool doubleDigit = true; // In generating, the first digit left of the check digit is doubled

    for (int i = static_cast<int>(num.length()) - 1; i >= 0; --i) {
        int digit = num[i] - '0';

        if (doubleDigit) {
            digit *= 2;
            if (digit > 9) {
                digit -= 9;
            }
        }

        sum += digit;
        doubleDigit = !doubleDigit;
    }

    int remainder = sum % 10;
    return (remainder == 0) ? 0 : (10 - remainder);
}

int main() {
    std::cout << "==========================================\n";
    std::cout << "    Luhn Algorithm (Mod 10) Validator     \n";
    std::cout << "==========================================\n\n";

    std::vector<std::string> testNumbers = {
        "79927398713",        // Valid test case from ISO/IEC 7812
        "79927398710",        // Invalid
        "49927398716",        // Valid Visa pattern
        "4992-7398-717",      // Invalid (single digit tamper)
        "1234 5678 1234 5670",// Valid test card
        "1234 5678 1234 5671" // Invalid
    };

    std::cout << "--- Validating Numbers ---\n";
    for (const auto& num : testNumbers) {
        bool valid = isValidLuhn(num);
        std::cout << "Number: " << num
                  << " -> " << (valid ? "[VALID]" : "[INVALID]") << "\n";
    }

    std::cout << "\n--- Check Digit Generation Demo ---\n";
    std::string partial = "7992739871"; // Payload without check digit
    int checkDigit = generateCheckDigit(partial);
    std::string fullNumber = partial + std::to_string(checkDigit);

    std::cout << "Payload           : " << partial << "\n";
    std::cout << "Calculated Check  : " << checkDigit << "\n";
    std::cout << "Full Number       : " << fullNumber << "\n";
    std::cout << "Validity Check    : " << (isValidLuhn(fullNumber) ? "CONFIRMED VALID" : "FAILED") << "\n";

    return 0;
}
