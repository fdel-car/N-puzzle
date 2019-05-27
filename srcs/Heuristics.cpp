#include "Heuristics.hpp"

static uMapConflicts::iterator findLargestListInMap(uMapConflicts &conflicts) {
  return std::max_element(conflicts.begin(), conflicts.end(),
                          [](const std::pair<u_char, std::list<u_char>> &lhs,
                             const std::pair<u_char, std::list<u_char>> &rhs) {
                            return lhs.second.size() < rhs.second.size();
                          });
}

static int conflictCount(uMapConflicts &conflicts) {
  int count = 0;
  uMapConflicts::iterator it = findLargestListInMap(conflicts);
  while (it != conflicts.end() && !it->second.empty()) {
    it->second.clear();
    for (auto &pair : conflicts) {
      if (pair.first == it->first) continue;
      pair.second.remove(it->first);
    }
    it = findLargestListInMap(conflicts);
    count++;
  }
  return count;
}

int Heuristics::manhattanDistance(const std::vector<u_char> &tiles) {
  if (puzzleInstance == nullptr)
    throw std::runtime_error(
        "missing puzzle instance needed for the finalGrid data.");
  int dist = 0;
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      u_char tile = tiles[x + y * Puzzle::N];
      if (tile == 0 || tile == puzzleInstance->finalGrid[x + y * Puzzle::N])
        continue;
      const std::array<int, 2> finalCoords =
          Node::getValueCoords(puzzleInstance->finalGrid, tile);
      dist += abs(x - finalCoords[0]) + abs(y - finalCoords[1]);
    }
  }
  return dist;
}

int Heuristics::_rowConflicts(const std::vector<u_char> &tiles) {
  int rowConf = 0;
  for (int y = 0; y < Puzzle::N; y++) {
    uMapConflicts conflicts;
    for (int tjX = 0; tjX < Puzzle::N; tjX++) {
      u_char tj = tiles[tjX + y * Puzzle::N];
      if (tj == 0 || tj == puzzleInstance->finalGrid[tjX + y * Puzzle::N])
        continue;
      const std::array<int, 2> tjFinalCoords =
          Node::getValueCoords(puzzleInstance->finalGrid, tj);
      if (tjFinalCoords[1] != y) continue;
      conflicts[tj] = std::list<u_char>();
      for (int tkX = 0; tkX < Puzzle::N; tkX++) {
        u_char tk = tiles[tkX + y * Puzzle::N];
        if (tk == 0) continue;
        const std::array<int, 2> tkFinalCoords =
            Node::getValueCoords(puzzleInstance->finalGrid, tk);
        if (tkFinalCoords[1] != y ||
            (tkFinalCoords[0] <= tjFinalCoords[0] && tkX <= tjX) ||
            (tkFinalCoords[0] >= tjFinalCoords[0] && tkX >= tjX))
          continue;
        conflicts[tj].push_back(tk);
      }
    }
    rowConf += conflictCount(conflicts);
  }
  return rowConf;
}

int Heuristics::_colConflicts(const std::vector<u_char> &tiles) {
  int colConf = 0;
  for (int x = 0; x < Puzzle::N; x++) {
    uMapConflicts conflicts;
    for (int tjY = 0; tjY < Puzzle::N; tjY++) {
      u_char tj = tiles[x + tjY * Puzzle::N];
      if (tj == 0 || tj == puzzleInstance->finalGrid[x + tjY * Puzzle::N])
        continue;
      const std::array<int, 2> tjFinalCoords =
          Node::getValueCoords(puzzleInstance->finalGrid, tj);
      if (tjFinalCoords[0] != x) continue;
      conflicts[tj] = std::list<u_char>();
      for (int tkY = 0; tkY < Puzzle::N; tkY++) {
        u_char tk = tiles[x + tkY * Puzzle::N];
        if (tk == 0) continue;
        const std::array<int, 2> tkFinalCoords =
            Node::getValueCoords(puzzleInstance->finalGrid, tk);
        if (tkFinalCoords[0] != x ||
            (tkFinalCoords[1] <= tjFinalCoords[1] && tkY <= tjY) ||
            (tkFinalCoords[1] >= tjFinalCoords[1] && tkY >= tjY))
          continue;
        conflicts[tj].push_back(tk);
      }
    }
    colConf += conflictCount(conflicts);
  }
  return colConf;
}

int Heuristics::linearConflicts(const std::vector<u_char> &tiles) {
  if (puzzleInstance == nullptr)
    throw std::runtime_error(
        "Missing puzzle instance needed for the finalGrid data.");
  int linearConf = _rowConflicts(tiles);
  linearConf += _colConflicts(tiles);
  return manhattanDistance(tiles) + linearConf * 2;
}

Puzzle *Heuristics::puzzleInstance = nullptr;
