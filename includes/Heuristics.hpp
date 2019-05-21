#pragma once

#include "Puzzle.hpp"

class Heuristics {
 public:
  static Puzzle *puzzleInstance;

  static int manhattanDistance(const std::vector<u_char> &tiles);

 private:
  Heuristics(void);
  Heuristics(Heuristics const &src);
  virtual ~Heuristics(void);

  Heuristics &operator=(Heuristics const &rhs);
};
