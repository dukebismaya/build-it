#include "account.h"
#include <vector>
int main() {

  account_log("Welcome To Build It bank\n");
  std::vector<std::string> options{"1. Create Account\n", "2. Deposit\n",
                                   "3. Withdraw\n", "4. Check Balance\n"};
  for (const auto &c : options) {
    std::cout << c;
  }
  while (true) {
    int choice_id{};
    // Will be implemented
  }
  return 0;
}