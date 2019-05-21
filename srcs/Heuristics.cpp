#include "Heuristics.hpp"

int Heuristics::manhattanDistance(const std::vector<u_char> &tiles) {
  if (puzzleInstance == nullptr)
    throw std::runtime_error(
        "Missing puzzle instance needed for the finalGrid data.");
  int cost = 0;
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      if (tiles[x + y * Puzzle::N] == 0) continue;
      if (tiles[x + y * Puzzle::N] !=
          puzzleInstance->finalGrid[x + y * Puzzle::N]) {
        const std::array<int, 2> coords = Node::getValueCoords(
            puzzleInstance->finalGrid, tiles[x + y * Puzzle::N]);
        cost += abs(x - coords[0]) + abs(y - coords[1]);
      }
    }
  }
  return cost;
}

Puzzle *Heuristics::puzzleInstance = nullptr;
