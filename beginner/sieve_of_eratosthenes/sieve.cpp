#include "sieve.h"
#include <vector>

auto primes(int num) -> std::vector<int> {
  std::vector<int> res;
  if (num < 2) {
    return res;
  }

  std::vector<bool> is_prime(num + 1, true);
  is_prime[0] = is_prime[1] = false;

  for (int p = 2; p * p <= num; ++p) {
    if (is_prime[p]) {
      for (int i = p * p; i <= num; i += p) {
        is_prime[i] = false;
      }
    }
  }

  for (int p = 2; p <= num; ++p) {
    if (is_prime[p]) {
      res.push_back(p);
    }
  }

  return res;
}