#include "account.h"
#include <cstdint>
auto Account::withdraw(double amount) -> bool {
  if (amount < 0) {
    account_log("Amount can't be negative\n");
    return false;
  }
  if (amount > balance) {
    account_log("Insufficient Balance\n");
    return false;
  }
  balance -= amount;
  return true;
}

auto Account::deposit(double amount) -> bool {
  if (amount <= 0) {
    account_log("Amount must be positive\n");
    return false;
  }
  balance += amount;
  return true;
}

auto Account::get_balance() -> void {
  account_log(std::format("Current Balance: {}\n", balance));
}

auto Account::check_passwd(uint64_t ph_no) -> bool {
  return ph_no == this->ph_no;
}