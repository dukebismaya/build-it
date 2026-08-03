#include "queen_attack.h"

auto chess_board::can_attack() const -> bool {
  return white_.first == black_.first || white_.second == black_.second ||
         (std::abs(black_.first - white_.first) ==
          std::abs(black_.second - white_.second));
}
