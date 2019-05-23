#pragma once

#include <algorithm>
#include <chrono>
#include <list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "Node.hpp"

class Puzzle {
 public:
  Puzzle(const std::vector<u_char> &firstGrid);
  virtual ~Puzzle(void);

  const std::vector<u_char> finalGrid;

  static int N;
  static int totalSize;
  static int nbrLength;

  int Solve(void);

 private:
  size_t _moveCount = 0;
  size_t _timeComplexity = 0;
  size_t _sizeComplexity = 0;
  std::priority_queue<Node *, std::vector<Node *>, NodePtrGreaterThan>
      _openedSet;
  std::unordered_map<std::string, Node *> _lookupTable;
  std::unordered_map<std::string, std::vector<u_char>> _closedSet;
  std::chrono::system_clock::time_point _start;
  std::chrono::system_clock::time_point _end;

  static const std::array<int, 4> _rowOffset;
  static const std::array<int, 4> _colOffset;

  Puzzle(void);
  Puzzle(Puzzle const &src);

  const std::vector<u_char> _initFinalGrid(void) const;
  void _printPath(const std::vector<u_char> &tiles);
  void _printPath(const Node *node);
  bool _isSwapSafe(const std::array<int, 2> &emptyTileCoords,
                   const std::array<int, 2> &emptyTileSwapDir);

  Puzzle &operator=(Puzzle const &rhs);
};
