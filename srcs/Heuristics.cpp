#include "Heuristics.hpp"

int Heuristics::manhattanDistance(const std::vector<u_char> &tiles) {
  if (puzzle == nullptr)
    throw std::runtime_error(
        "Puzzle static pointer not assigned in the heuristics class.");
  int cost = 0;
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      if (tiles[x + y * Puzzle::N] == 0) continue;
      if (tiles[x + y * Puzzle::N] != puzzle->finalGrid[x + y * Puzzle::N]) {
        const std::array<int, 2> coords =
            Node::getValueCoords(puzzle->finalGrid, tiles[x + y * Puzzle::N]);
        cost += abs(x - coords[0]) + abs(y - coords[1]);
      }
    }
  }
  return cost;
}

Puzzle *Heuristics::puzzle = nullptr;
