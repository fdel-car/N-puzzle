#include <sys/stat.h>
#include "InputHandler.hpp"
#include "Puzzle.hpp"

int main(int ac, char **av) {
  try {
    InputHandler iHandler(ac, av);
    iHandler.parseTiles();
    if (!iHandler.startGrid.empty()) return Puzzle(iHandler.startGrid).Solve();
    return EXIT_FAILURE;
  } catch (const std::exception &err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
