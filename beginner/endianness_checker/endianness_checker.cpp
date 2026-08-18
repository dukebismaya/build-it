#include <cstdint>
#include <iostream>

auto isLittleEndian() -> bool {
  uint16_t testValue = 0x0001;
  auto testValAddr = reinterpret_cast<uint8_t *>(&testValue);
  return *testValAddr == 1;
}

int main() {
  if (isLittleEndian()) {
    std::cout << "System is Little-Endian (Intel/AMD style)\n";
  } else {
    std::cout << "System is Big-Endian (Network/Motorola style)\n";
  }
  return 0;
}