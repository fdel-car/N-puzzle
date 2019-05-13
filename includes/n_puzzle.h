#pragma once

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <list>

#define N 3

typedef struct Node {
  Node *parent = nullptr;
  int gScore;
  int hScore;
  int fScore;
  std::array<std::array<u_char, N>, N> tiles;
  std::array<int, 2> emptyTileCoords;
  static const int numLength;
} Node;

const int Node::numLength = std::to_string(N * N).length();

inline bool operator<(const Node &lhs, const Node &rhs) {
  return lhs.fScore < rhs.fScore;
}

inline std::ostream &operator<<(std::ostream &os, const Node &node) {
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      if (x != 0 && x != N) os << ' ';
      os << std::setw(Node::numLength) << static_cast<int>(node.tiles[x][y]);
    }
    os << std::endl;
  }
  return os;
}
