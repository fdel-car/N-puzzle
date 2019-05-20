#pragma once

#include <algorithm>
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

  int solve(void);

 private:
  int _moveCount = 0;
  std::priority_queue<Node *, std::vector<Node *>, NodePtrCmp> _openSet;
  std::unordered_map<std::string, Node *> _lookupTable;
  std::unordered_set<Node *, std::hash<Node *>, NodePtrEqual> _closeSet;

  static const std::array<int, 4> _rowOffset;
  static const std::array<int, 4> _colOffset;

  Puzzle(void);
  Puzzle(Puzzle const &src);

  const std::vector<u_char> _initFinalGrid(void) const;
  void _printPath(const Node *node, bool isLastNode);
  bool _isSwapSafe(const std::array<int, 2> &emptyTileCoords,
                   const std::array<int, 2> &emptyTileSwapDir);

  Puzzle &operator=(Puzzle const &rhs);
};
