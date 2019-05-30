#include "Puzzle.hpp"
#include "Heuristics.hpp"

Puzzle::Puzzle(const std::vector<u_char>& startGrid)
    : finalGrid(_initFinalGrid()) {
  Heuristics::puzzleInstance = this;
  if (!_isSolvable(startGrid))
    throw std::logic_error("this puzzle is not solvable.");
  Node* firstNode = new Node(startGrid);
  firstNode->computeHeuristic();
  _openedSet.push(firstNode);
  _lookupTable[firstNode->ID] = firstNode;
}

Puzzle::~Puzzle(void) {
  // Delete all nodes in containers
  _lookupTable.clear();
  while (!_openedSet.empty()) {
    delete _openedSet.top();
    _openedSet.pop();
  }
  _closedSet.clear();
}

int Puzzle::_inversionCount(const std::vector<u_char>& tiles) const {
  int invCount = 0;
  for (int i = 0; i < totalSize; i++) {
    u_char tile = tiles[i];
    if (tile == 0) continue;
    for (int j = i + 1; j < totalSize; j++) {
      if (tiles[j] < tile && tiles[j] != 0) invCount++;
    }
  }
  return invCount;
}

bool Puzzle::_isSolvable(const std::vector<u_char>& startGrid) const {
  int startInv = _inversionCount(startGrid);
  int finalInv = _inversionCount(finalGrid);

  if (Puzzle::N % 2 == 0) {
    const std::array<int, 2> emptyStartCoords =
        Node::getValueCoords(startGrid, 0);
    const std::array<int, 2> emptyFinalCoords =
        Node::getValueCoords(finalGrid, 0);
    startInv += emptyStartCoords[1];
    finalInv += emptyFinalCoords[1];
  }

  return startInv % 2 == finalInv % 2;
}

const std::vector<u_char> Puzzle::_initFinalGrid(void) const {
  std::vector<u_char> vector(totalSize, 0);

  if (_goalPattern == Snail) {
    u_char value = 1;
    int x = 0, y = 0;
    int xInc = 1, yInc = 0;
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
      if (value == N * N || value == 0) break;
    }
  } else if (_goalPattern == Classic) {
    for (int i = 0; i < totalSize - 1; i++) vector[i] = i + 1;
  }

  return vector;
}

void Puzzle::_writeToFile(void) const {
  std::ofstream file;
  file.open("./public/data.json");
  if (file.is_open()) {
    file << _dataOutput;
    file.close();
  }
}

void Puzzle::_printPath(const std::vector<u_char>& tiles) {
  if (tiles.size() == 0) return;
  Node tmpNode(tiles);
  _printPath(_closedSet[tmpNode.ID]);
  std::cout << tmpNode << std::endl;

  _dataOutput += "    [";
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      if (x != 0 && x != Puzzle::N) _dataOutput += ", ";
      int tile = tiles[x + y * Puzzle::N];
      _dataOutput += std::to_string(tile);
    }
    if (y != Puzzle::N - 1) _dataOutput += ", ";
  }
  _dataOutput += "],\n";

  _moveCount++;
}

// Should be used for the last node in the path only.
void Puzzle::_printPath(const Node* node) {
  _dataOutput +=
      "{\n  \"size\": " + std::to_string(Puzzle::N) + ",\n  \"states\": [\n";

  _printPath(node->parentTiles);
  std::cout << *node << std::endl;

  _dataOutput += "    [";
  for (int y = 0; y < Puzzle::N; y++) {
    for (int x = 0; x < Puzzle::N; x++) {
      if (x != 0 && x != Puzzle::N) _dataOutput += ", ";
      int tile = node->tiles[x + y * Puzzle::N];
      _dataOutput += std::to_string(tile);
    }
    if (y != Puzzle::N - 1) _dataOutput += ", ";
  }
  _dataOutput +=
      "]\n  ],\n  \"timeComplexity\": " + std::to_string(_timeComplexity) +
      ",\n  \"sizeComplexity\": " + std::to_string(_sizeComplexity) + ",\n";

  // Set french locale for thousands separator
  std::locale oldLocale =
      std::cout.imbue(std::locale(std::cout.getloc(), new FrenchFacet));

  std::cout << "Total number of states ever selected: " << _timeComplexity
            << '.' << std::endl;
  std::cout << "Maximum number of states ever represented in memory: "
            << _sizeComplexity << '.' << std::endl;
  std::cout << "Number of moves required: " << _moveCount << '.' << std::endl;
  auto duration = std::chrono::duration<double>(_end - _start);
  _dataOutput +=
      "  \"searchTime\": " + std::to_string(duration.count()) + "\n}";
  std::cout << std::fixed << "Search time: " << duration.count() << " seconds."
            << std::endl;

  // Reset locale
  std::cout.imbue(oldLocale);

  _writeToFile();
}

bool Puzzle::_isSwapSafe(const std::array<int, 2>& emptyTileCoords,
                         const std::array<int, 2>& emptyTileSwapDir) const {
  int x = emptyTileCoords[0] + emptyTileSwapDir[0];
  int y = emptyTileCoords[1] + emptyTileSwapDir[1];
  if (x < 0 || x >= N || y < 0 || y >= N) return false;
  return true;
}

int Puzzle::Solve(void) {
  _start = std::chrono::system_clock::now();

  while (!_openedSet.empty()) {
    Node* node = _openedSet.top();

    if (_sizeComplexity < _openedSet.size())
      _sizeComplexity = _openedSet.size();

    if ((currAlgorithm != UniformCost && node->hScore == 0) ||
        node->tiles == finalGrid) {
      _end = std::chrono::system_clock::now();
      _printPath(node);
      return EXIT_SUCCESS;
    }
    _openedSet.pop();
    if (_closedSet.find(node->ID) != _closedSet.end()) {
      delete node;
      continue;
    }

    for (int i = 0; i < 4; i++) {
      std::array<int, 2> emptyTileSwapDir = {_rowOffset[i], _colOffset[i]};
      if (emptyTileSwapDir == node->parentOffset ||
          !_isSwapSafe(node->emptyTileCoords, emptyTileSwapDir))
        continue;
      Node* neighbor = nullptr;
      try {
        neighbor = new Node(node, emptyTileSwapDir);
      } catch (const std::bad_alloc& err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return EXIT_FAILURE;
      }

      if (_closedSet.find(neighbor->ID) != _closedSet.end()) {
        delete neighbor;
        continue;
      }

      auto it = _lookupTable.find(neighbor->ID);
      if (it == _lookupTable.end()) {
        neighbor->computeHeuristic();
        _openedSet.push(neighbor);
        _lookupTable[neighbor->ID] = neighbor;
        _timeComplexity++;
      } else {
        if (neighbor->gScore >= ((*it).second)->gScore) {
          delete neighbor;
          continue;
        }
        neighbor->hScore = ((*it).second)->hScore;
        neighbor->fScore = neighbor->gScore + neighbor->hScore;
        _openedSet.push(neighbor);
        _lookupTable[neighbor->ID] = neighbor;
        // This update could be *dangerous* because the priority queue is not
        // reordered, so another approach was used just above

        /*
        ((*it).second)->parent = node;
        ((*it).second)->gScore = neighbor->gScore;
        ((*it).second)->fScore =
            ((*it).second)->gScore + ((*it).second)->hScore;
        */

        // One solution to reorder the priority queue after an update,
        // a bit too hackish and slow

        /*
        std::make_heap(const_cast<Node**>(&_openedSet.top()),
                       const_cast<Node**>(&_openedSet.top() +
        _openedSet.size()), NodePtrGreaterThan());
        */
      }
    }
    _closedSet[node->ID] = node->parentTiles;
    delete node;
  }
  return EXIT_FAILURE;
}

bool Puzzle::updateGoalPattern(char flag) {
  auto itGMap = _goalMap.find(flag);
  if (itGMap != _goalMap.end()) {
    _goalPattern = itGMap->second;
    return true;
  }
  return false;
}

std::unordered_map<char, Puzzle::GoalPattern> Puzzle::_getGoalMap(void) {
  std::unordered_map<char, GoalPattern> uMap;

  uMap['s'] = Snail;
  uMap['c'] = Classic;

  return uMap;
}

std::unordered_map<char, Puzzle::GoalPattern> Puzzle::_goalMap = _getGoalMap();

std::unordered_map<char, Puzzle::Algorithm> Puzzle::_getAlgoMap(void) {
  std::unordered_map<char, Algorithm> uMap;

  uMap['A'] = AStar;
  uMap['g'] = Greedy;
  uMap['u'] = UniformCost;

  return uMap;
}

std::unordered_map<char, Puzzle::Algorithm> Puzzle::algoMap = _getAlgoMap();

int Puzzle::N = 0;
int Puzzle::totalSize = 0;
int Puzzle::nbrLength = 0;
Puzzle::GoalPattern Puzzle::_goalPattern = _goalMap['s'];
Puzzle::Algorithm Puzzle::currAlgorithm = algoMap['a'];

const std::array<int, 4> Puzzle::_rowOffset = {1, 0, -1, 0};
const std::array<int, 4> Puzzle::_colOffset = {0, 1, 0, -1};
