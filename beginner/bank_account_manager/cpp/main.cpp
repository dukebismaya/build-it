#include "account.h"
#include <cstdint>
#include <memory>
#include <vector>

int main() {

  account_log("Welcome To Build It bank\n");
  std::vector<std::string> options{"1. Create Account\n", "2. Deposit\n",
                                   "3. Withdraw\n", "4. Check Balance\n",
                                   "5. Exit\n"};
  for (const auto &c : options) {
    std::cout << c;
  }
  std::unique_ptr<Account> account;
  while (true) {
    int choice_id{};
    std::cin >> choice_id;
    switch (choice_id) {
    case 1: {
      std::string name;
      int age;
      uint64_t ph_no;
      double balance;
      std::cout << "Enter your name: ";
      std::cin >> name;
      std::cout << "Enter your age: ";
      std::cin >> age;
      if (age < 18) {
        account_log("You must be an adult to create an account\n");
        break;
      }
      std::cout << "Enter your phone no: ";
      std::cin >> ph_no;
      std::cout << "Enter the deposit amount: ";
      std::cin >> balance;
      account = std::make_unique<Account>(name, age, ph_no, balance);
      break;
    }
    case 2: {
      if (account) {
        std::cout << "Enter amount to deposit: ";
        double amount;
        std::cin >> amount;
        account->deposit(amount);
      }
      break;
    }

    case 3: {
      if (account) {
        std::cout << "Enter amount to withdraw: ";
        double amount;
        std::cin >> amount;
        account->withdraw(amount);
      }
      break;
    }
    case 4: {
      if (account) {
        account_log("Enter your phone number to check balance\n");
        uint64_t ph_no;
        std::cin >> ph_no;
        if (account->check_passwd(ph_no))
          account->get_balance();
        else
          account_log("Authentication Failed. Invalid Passcode\n");
      }
      break;
    }
    case 5: {
      account_log("Exited successfully.\n");
      std::cout << std::flush;
      std::exit(0);
    }
    default:
      account_log("Invalid Options. Please try again\n");
      break;
    }
  }
  return 0;
}