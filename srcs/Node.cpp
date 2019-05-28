#include "Node.hpp"
#include "Heuristics.hpp"
#include "Puzzle.hpp"

static const std::string zeroFont = "\033[36;1m";
static const std::string resetFont = "\033[0m";

Node::Node(const std::vector<u_char> &startGrid) {
  tiles.resize(Puzzle::totalSize);
  std::memcpy(tiles.data(), startGrid.data(),
              sizeof(u_char) * Puzzle::totalSize);
  ID = std::string(tiles.begin(), tiles.end());
  gScore = 0;
  emptyTileCoords = getValueCoords(tiles, 0);
}

Node::Node(Node *parent, const std::array<int, 2> &emptyTileSwapDir)
    : parentTiles(parent->tiles) {
  assert(parent != nullptr);
  tiles.resize(Puzzle::totalSize);
  std::memcpy(tiles.data(), (parentTiles).data(),
              sizeof(u_char) * Puzzle::totalSize);
  gScore = parent->gScore + 1;

  // Move tile to the next position
  int oldX = parent->emptyTileCoords[0];
  int oldY = parent->emptyTileCoords[1];
  int newX = oldX + emptyTileSwapDir[0];
  int newY = oldY + emptyTileSwapDir[1];
  std::swap(tiles[oldX + oldY * Puzzle::N], tiles[newX + newY * Puzzle::N]);
  ID = std::string(tiles.begin(), tiles.end());
  emptyTileCoords = {newX, newY};

  // Used to avoid going back when this node will be checked
  parentOffset = {-emptyTileSwapDir[0], -emptyTileSwapDir[1]};
}

Node::~Node(void) {}

void Node::computeHeuristic(void) {
  hScore = currHeuristic(tiles);
  fScore = gScore + hScore;
}

const std::array<int, 2> Node::getValueCoords(const std::vector<u_char> &tiles,
                                              u_char value) {
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      if (tiles[x + y * Puzzle::N] == value) return {x, y};
    }
  }
  assert(false);
  return {-1, -1};  // Will never happen but make the compiler happy
}

std::unordered_map<std::string, Node::HeuristicFunction> Node::_getHeuristicMap(
    void) {
  std::unordered_map<std::string, Node::HeuristicFunction> uMap;

  uMap["-m"] = &Heuristics::manhattanDistance;
  uMap["-l"] = &Heuristics::linearConflicts;

  return uMap;
}

std::unordered_map<std::string, Node::HeuristicFunction> Node::hMap =
    _getHeuristicMap();

Node::HeuristicFunction Node::currHeuristic = hMap["-l"];

bool Node::operator==(const Node &rhs) const { return ID == rhs.ID; }

std::ostream &operator<<(std::ostream &os, const Node &node) {
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      if (x != 0 && x != Puzzle::N) os << ' ';
      int tile = node.tiles[x + y * Puzzle::N];
      if (tile == 0) {
        os << zeroFont << std::setw(Puzzle::nbrLength) << tile << resetFont;
      } else
        os << std::setw(Puzzle::nbrLength) << tile;
    }
    os << std::endl;
  }
  // // ID debug
  // for (u_char c : node.ID) {
  //   c += 48;
  //   os << c;
  // }
  return os;
}
