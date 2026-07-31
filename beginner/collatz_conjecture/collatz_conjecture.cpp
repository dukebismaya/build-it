#include <cstdint>
#include <iostream>
#include <stdexcept>

auto steps(int64_t num) -> int {
  if (num <= 0)
    throw std::domain_error("Number must be a positive integer");
  int steps{};
  while (num > 1) {
    if (num % 2 == 0)
      num /= 2;
    else
      num = 3 * num + 1;
    steps++;
  }
  return steps;
}

int main() {
  std::cout << "Enter a number: ";
  int64_t num{};
  if (!(std::cin >> num)) {
    std::cerr << "Invalid input. Please enter an integer.\n";
    return 1;
  }

  try {
    std::cout
        << "Total steps required to become 1 following collatz conjecture is: "
        << steps(num) << "\n";
  } catch (const std::domain_error &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}