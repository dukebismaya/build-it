#include <cstdint>
#include <iostream>

auto isLittleEndian() -> bool {
  uint16_t testValue = 0x0001;
  return false;
}

int main() {
  if (isLittleEndian()) {
    std::cout << "System is Little-Endian (Intel/AMD style)\n";
  } else {
    std::cout << "System is Big-Endian (Network/Motorola style)\n";
  }
  return 0;
}