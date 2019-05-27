#pragma once

#include <iomanip>
#include <iostream>
#include <vector>
#include <array>

class Node {
 public:
  Node(const std::vector<u_char> &startGrid);
  Node(Node *parent, const std::array<int, 2> &emptyTileSwapDir);
  virtual ~Node(void);

  std::string ID;
  int gScore;
  int hScore;
  int fScore;
  std::vector<u_char> tiles;
  std::vector<u_char> parentTiles;
  std::array<int, 2> emptyTileCoords;
  std::array<int, 2> parentOffset;

  void computeHeuristic(void);

  static const std::array<int, 2> getValueCoords(
      const std::vector<u_char> &tiles, u_char value);

  bool operator==(const Node &rhs) const;

 private:
  Node(void);
  Node(Node const &src);

  Node &operator=(Node const &rhs);
};

template <>
struct std::hash<Node> {
  size_t operator()(const Node &node) const { return hash<string>()(node.ID); }
};

template <>
struct std::hash<Node *> {
  std::size_t operator()(const Node *node) const { return hash<Node>()(*node); }
};

struct NodePtrEqual {
  bool operator()(const Node *lhs, const Node *rhs) const {
    return *lhs == *rhs;
  }
};

struct NodePtrGreaterThan {
  bool operator()(const Node *lhs, const Node *rhs) const {
    return lhs->fScore > rhs->fScore;
  }
};

std::ostream &operator<<(std::ostream &os, const Node &node);
