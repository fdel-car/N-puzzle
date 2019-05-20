#include "Puzzle.hpp"
#include "Heuristics.hpp"

Puzzle::Puzzle(const std::vector<u_char>& firstGrid)
    : finalGrid(_initFinalGrid()) {
  Heuristics::puzzle = this;
  Node* firstNode = new Node(firstGrid);
  _openSet.push(firstNode);
  _lookupTable[firstNode->ID] = firstNode;
}

Puzzle::~Puzzle(void) {
  // Delete all nodes in containers
}

const std::vector<u_char> Puzzle::_initFinalGrid(void) const {
  u_char value = 1;
  int x = 0, y = 0;
  int xInc = 1, yInc = 0;
  std::vector<u_char> vector(totalSize, 0);

  while (true) {
    vector[x + y * N] = value;
    if (x + xInc == N || x + xInc < 0 ||
        (xInc != 0 && vector[(x + xInc) + y * N] != 0)) {
      yInc = xInc;
      xInc = 0;
    } else if (y + yInc == N || y + yInc < 0 ||
               (yInc != 0 && vector[x + (y + yInc) * N] != 0)) {
      xInc = -yInc;
      yInc = 0;
    }
    x += xInc;
    y += yInc;
    value++;
    if (value == N * N) break;
  }
  return vector;
}

void Puzzle::_printPath(const Node* node, bool isLastNode) {
  if (node == nullptr) return;
  _printPath(node->parent, false);
  std::cout << *node << std::endl;
  if (!isLastNode)
    _moveCount++;
  else
    std::cout << "Number of required moves: " << _moveCount << std::endl;
}

bool Puzzle::_isSwapSafe(const std::array<int, 2>& emptyTileCoords,
                         const std::array<int, 2>& emptyTileSwapDir) {
  int x = emptyTileCoords[0] + emptyTileSwapDir[0];
  int y = emptyTileCoords[1] + emptyTileSwapDir[1];
  if (x < 0 || x >= N || y < 0 || y >= N) return false;
  return true;
}

int Puzzle::solve(void) {
  while (!_openSet.empty()) {
    Node* node = _openSet.top();
    if (node->hScore == 0) {
      _printPath(node, true);
      return EXIT_SUCCESS;
    }
    _openSet.pop();
    _closeSet.insert(node);

    for (int i = 0; i < 4; i++) {
      std::array<int, 2> emptyTileSwapDir = {_rowOffset[i], _colOffset[i]};
      if (!_isSwapSafe(node->emptyTileCoords, emptyTileSwapDir)) continue;
      Node* neighbor = new Node(node, emptyTileSwapDir);

      if (_closeSet.find(neighbor) != _closeSet.end()) {
        delete neighbor;
        continue;
      }

      auto it = _lookupTable.find(neighbor->ID);
      if (it == _lookupTable.end()) {
        neighbor->computeHeuristic();
        _openSet.push(neighbor);
        _lookupTable[neighbor->ID] = neighbor;
      } else if (neighbor->gScore >= ((*it).second)->gScore) {
        delete neighbor;
        continue;
      } else {
        // this update could be *dangerous* the priority queue is not reordered,
        // look into this
        ((*it).second)->parent = node;
        ((*it).second)->gScore = neighbor->gScore;
        ((*it).second)->fScore =
            ((*it).second)->gScore + ((*it).second)->hScore;
      }
    }
  }
  return EXIT_FAILURE;
}

int Puzzle::N = 0;
int Puzzle::totalSize = 0;
int Puzzle::nbrLength = 0;

const std::array<int, 4> Puzzle::_rowOffset = {1, 0, -1, 0};
const std::array<int, 4> Puzzle::_colOffset = {0, 1, 0, -1};
