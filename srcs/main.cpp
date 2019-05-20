#include <fstream>
#include "InputHandler.hpp"
#include "Puzzle.hpp"

int main(int ac, char **av) {
  std::vector<u_char> firstGrid;

  if (ac == 1)
    firstGrid = InputHandler(std::cin).firstGrid;
  else {
    std::filebuf fb;
    if (fb.open(av[1], std::ios::in)) {
      std::istream is(&fb);
      firstGrid = InputHandler(is).firstGrid;
      fb.close();
    } else {
      std::cerr << "Could not open " << av[1] << '.' << std::endl;
      return EXIT_FAILURE;
    }
  }
  Puzzle puzzle(firstGrid);
  return puzzle.solve();
}
