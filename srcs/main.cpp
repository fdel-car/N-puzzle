#include "Puzzle.hpp"

int main(void) {
  std::vector<u_char> firstGrid(9, 0);

  // 1st column
  firstGrid[0 + 0 * 3] = 4;
  firstGrid[0 + 1 * 3] = 3;
  firstGrid[0 + 2 * 3] = 5;

  // 2nd column
  firstGrid[1 + 0 * 3] = 1;
  firstGrid[1 + 1 * 3] = 8;
  firstGrid[1 + 2 * 3] = 2;

  // 3rd column
  firstGrid[2 + 0 * 3] = 6;
  firstGrid[2 + 1 * 3] = 0;
  firstGrid[2 + 2 * 3] = 7;

  Puzzle::N = 3;
  Puzzle::totalSize = Puzzle::N * Puzzle::N;
  Puzzle::nbrLength = std::to_string(Puzzle::totalSize).length();

  Puzzle puzzle(firstGrid);
  return puzzle.solve();
}
