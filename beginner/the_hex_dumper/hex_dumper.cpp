#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

void printHexDump(const std::vector<uint8_t> &buffer) {
  for (const auto packet : buffer) {
    std::cout << std::setw(2) << std::setfill('0') << std::hex
              << static_cast<int>(packet) << " ";
  }
  std::cout << std::setfill(' ');
}

int main() {
  std::vector<uint8_t> dummyPacket{255, 10, 128, 0, 42};
  printHexDump(dummyPacket);
  return 0;
}