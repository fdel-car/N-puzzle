#include "InputHandler.hpp"
#include "Puzzle.hpp"

int main(int ac, char **av) {
  Puzzle *puzzleInstance = nullptr;
  try {
    InputHandler iHandler(ac, av);
    iHandler.parseTiles();
    if (!iHandler.startGrid.empty())
      puzzleInstance = new Puzzle(iHandler.startGrid);
  } catch (const std::exception &err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
  if (puzzleInstance == nullptr) return EXIT_FAILURE;
  return puzzleInstance->Solve();
}
