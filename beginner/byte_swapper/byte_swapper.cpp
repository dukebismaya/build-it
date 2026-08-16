#include <cstdint>
#include <iomanip>
#include <iostream>

uint16_t swapBytes16(uint16_t value) { return (value << 8) | (value >> 8); }

int main() {
  uint16_t networkPort = 0x1234;
  uint16_t hostPort = swapBytes16(networkPort);

  std::cout << std::hex << std::setfill('0');
  std::cout << "Original: 0x" << std::setw(4) << networkPort << "\n";
  std::cout << "Swapped:  0x" << std::setw(4) << hostPort << "\n";

  return 0;
}