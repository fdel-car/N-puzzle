#include <sys/stat.h>
#include <fstream>
#include "InputHandler.hpp"
#include "Puzzle.hpp"

int main(int ac, char **av) {
  try {
    std::vector<u_char> firstGrid;
    if (ac == 1)
      firstGrid = InputHandler(std::cin).firstGrid;
    else {
      std::filebuf fb;
      struct stat st;
      if (fb.open(av[1], std::ios::in)) {
        stat(av[1], &st);
        if (st.st_mode & S_IFDIR) {
          std::cerr << "Error: " << av[1] << " is a directory." << std::endl;
          return EXIT_FAILURE;
        }
        std::istream is(&fb);
        firstGrid = InputHandler(is).firstGrid;
        fb.close();
      } else {
        std::cerr << "Error: " << strerror(errno) << " (" << av[1] << ")."
                  << std::endl;
        return EXIT_FAILURE;
      }
    }
    return Puzzle(firstGrid).Solve();
  } catch (const std::exception &err) {
    std::cerr << "Error: " << err.what() << std::endl;
    return EXIT_FAILURE;
  }
}
