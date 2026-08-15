#include <cstdint>
#include <iostream>
#include <vector>

auto xorScramble(std::vector<uint8_t> &buffer, uint8_t key) {
  for (auto &byte : buffer) {
    byte ^= key;
  }
}

int main() {
  std::vector<uint8_t> payload = {10, 20, 30, 40, 50};
  uint8_t secret_key = 0xAA;
  xorScramble(payload, secret_key);
  std::cout << "Scrambled first byte: " << static_cast<int>(payload[0]) << "\n";

  xorScramble(payload, secret_key);
  std::cout << "Restored first byte: " << static_cast<int>(payload[0])
            << std::endl;

  return 0;
}