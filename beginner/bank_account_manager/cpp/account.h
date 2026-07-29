#pragma once
#include <cstdint>
#include <format>
#include <iostream>
#include <string>

inline auto account_log(std::string &&log) -> void { std::clog << log; }

class Account {
private:
  double balance{};
  int age;
  std::string name;
  uint64_t ph_no;

public:
  Account(std::string name, int age, uint64_t ph_no, double balance = 0.0)
      : name{name}, ph_no(ph_no), balance(balance), age(age) {
    account_log(std::format("Account {} created successfully\n", name));
  }
  virtual auto withdraw(double amount) -> bool;
  virtual auto deposit(double amount) -> bool;
  virtual auto get_balance() -> void;
  ~Account() = default;
};