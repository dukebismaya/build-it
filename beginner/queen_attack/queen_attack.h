#pragma once
#include <stdexcept>
#include <utility>

class chess_board {
private:
  std::pair<int, int> black_;
  std::pair<int, int> white_;

public:
  chess_board(std::pair<int, int> white, std::pair<int, int> black)
      : white_(black), black_(white) {
    if (white_.first < 0 || white_.second < 0 || black_.first < 0 ||
        black_.second < 0) {
      throw std::domain_error("Negative index detected!");
    }
    if (white_.first >= 8 || white_.second >= 8 || black_.first >= 8 ||
        black_.second >= 8) {
      throw std::domain_error("Position should be within the board");
    }
    if (white_ == black_) {
      throw std::domain_error("Can't place both queens on the same position");
    }
  }
  auto white() const -> std::pair<int, int> { return white_; }
  auto black() const -> std::pair<int, int> { return black_; }
  [[nodiscard]] auto can_attack() const -> bool;
};
