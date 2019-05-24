#pragma once

#include "Puzzle.hpp"

typedef std::unordered_map<u_char, std::list<u_char>> uMapConflicts;

class Heuristics {
 public:
  static Puzzle *puzzleInstance;

  static int manhattanDistance(const std::vector<u_char> &tiles);
  static int linearConflicts(const std::vector<u_char> &tiles);

 private:
  Heuristics(void);
  Heuristics(Heuristics const &src);
  virtual ~Heuristics(void);

  static int _rowConflicts(const std::vector<u_char> &tiles);
  static int _colConflicts(const std::vector<u_char> &tiles);

  Heuristics &operator=(Heuristics const &rhs);
};
