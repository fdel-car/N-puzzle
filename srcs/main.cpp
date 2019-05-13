#include "n_puzzle.h"

int rowOffset[4] = {1, 0, -1, 0};
int colOffset[4] = {0, 1, 0, -1};
std::array<std::array<u_char, N>, N> finalGrid;

bool isSwapSafe(const std::array<int, 2>& emptyTileCoords,
                const std::array<int, 2>& emptyTileSwapDir) {
  int x = emptyTileCoords[0] + emptyTileSwapDir[0];
  int y = emptyTileCoords[1] + emptyTileSwapDir[1];
  if (x < 0 || x >= N || y < 0 || y >= N) return false;
  return true;
}

const std::array<int, 2> getValueCoords(
    const std::array<std::array<u_char, N>, N>& tiles, u_char value) {
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      if (tiles[x][y] == value) return {x, y};
    }
  }

  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      if (x != 0 && x != N) std::cout << ' ';
      std::cout << std::setw(Node::numLength) << tiles[x][y];
    }
    std::cout << std::endl;
  }

  throw std::runtime_error("Looking for non existing value (" +
                           std::to_string(value) + ") inside the tiles.");
}

int manhattanDistance(const std::array<std::array<u_char, N>, N>& tiles) {
  int cost = 0;
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      if (tiles[x][y] == 0) continue;
      if (tiles[x][y] != finalGrid[x][y]) {
        const std::array<int, 2> coords =
            getValueCoords(finalGrid, tiles[x][y]);
        cost += abs(x - coords[0]) + abs(y - coords[1]);
      }
    }
  }
  return cost;
}

Node* createNode(const std::array<std::array<u_char, N>, N>& tiles) {
  Node* node = new Node;
  memcpy(&node->tiles, &tiles, sizeof(u_char) * N * N);
  node->gScore = 0;
  node->hScore = manhattanDistance(node->tiles);
  node->fScore = node->gScore + node->hScore;
  node->emptyTileCoords = getValueCoords(node->tiles, 0);
  return node;
}

Node* createNode(Node* parent, const std::array<int, 2>& emptyTileSwapDir) {
  if (!parent) throw std::runtime_error("No parent given to a new child node.");
  Node* node = new Node;
  node->parent = parent;
  memcpy(&node->tiles, &node->parent->tiles, sizeof(u_char) * N * N);
  node->gScore = node->parent->gScore + 1;

  // Move tile to the next position
  int oldX = node->parent->emptyTileCoords[0];
  int oldY = node->parent->emptyTileCoords[1];
  int newX = oldX + emptyTileSwapDir[0];
  int newY = oldY + emptyTileSwapDir[1];
  std::swap(node->tiles[oldX][oldY], node->tiles[newX][newY]);
  node->emptyTileCoords = {newX, newY};

  node->emptyTileCoords = getValueCoords(node->tiles, 0);
  return node;
}

void computeHeuristic(Node* node) {
  node->hScore = manhattanDistance(node->tiles);
  node->fScore = node->gScore + node->hScore;
}

void printPath(Node* node, bool isLastNode) {
  if (node == nullptr) return;
  printPath(node->parent, false);
  std::cout << *node;
  if (!isLastNode) std::cout << std::endl;
}

void initFinalGridState() {
  u_char value = 1;
  int x = 0, y = 0;
  int xInc = 1, yInc = 0;
  while (true) {
    finalGrid[x][y] = value;
    if (x + xInc == N || x + xInc < 0 ||
        (xInc != 0 && finalGrid[x + xInc][y] != 0)) {
      yInc = xInc;
      xInc = 0;
    } else if (y + yInc == N || y + yInc < 0 ||
               (yInc != 0 && finalGrid[x][y + yInc] != 0)) {
      xInc = -yInc;
      yInc = 0;
    }
    x += xInc;
    y += yInc;
    value++;
    if (value == N * N) break;
  }
}

int main(void) {
  std::list<Node*> openSet, closeSet, children;
  std::array<std::array<u_char, N>, N> firstGrid;

  // 1st column
  firstGrid[0][0] = 4;
  firstGrid[0][1] = 3;
  firstGrid[0][2] = 5;

  // 2nd column
  firstGrid[1][0] = 1;
  firstGrid[1][1] = 0;
  firstGrid[1][2] = 2;

  // 3rd column
  firstGrid[2][0] = 6;
  firstGrid[2][1] = 8;
  firstGrid[2][2] = 7;

  initFinalGridState();
  Node* firstNode = createNode(firstGrid);
  openSet.push_back(firstNode);

  while (!openSet.empty()) {
    Node* node = openSet.front();
    if (node->hScore == 0) {
      printPath(node, true);
      return 0;
    }
    openSet.pop_front();
    closeSet.push_front(node);
    for (int i = 0; i < 4; i++) {
      const std::array<int, 2> emptyTileSwapDir = {rowOffset[i], colOffset[i]};
      if (!isSwapSafe(node->emptyTileCoords, emptyTileSwapDir)) continue;
      Node* child = createNode(/* parent */
                               node, emptyTileSwapDir);

      // Verify if the node is already in the close set, if so, this child will
      // be ignored.
      auto it = std::find_if(
          closeSet.begin(), closeSet.end(),
          [child](Node* lhs) { return lhs->tiles == child->tiles; });
      if (it != closeSet.end()) {
        delete child;
        continue;
      }

      // Verify if the node is already in the open set, if so, update it if this
      // path required less moves.
      it = std::find_if(openSet.begin(), openSet.end(), [child](Node* lhs) {
        return lhs->tiles == child->tiles;
      });
      if (it == openSet.end()) {
        computeHeuristic(child);
        children.push_back(child);
      } else if (child->gScore >= (*it)->gScore) {
        delete child;
        continue;
      } else {
        (*it)->parent = node;
        (*it)->gScore = child->gScore;
      }
    }
    if (children.size() > 0) {
      children.sort([](Node* lhs, Node* rhs) { return *lhs < *rhs; });
      openSet.merge(children, [](Node* lhs, Node* rhs) { return *lhs < *rhs; });
    }
  }
}
