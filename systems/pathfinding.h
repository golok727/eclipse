#pragma once

#include "../glm/glm.hpp"

#include <vector>

namespace eclipse::systems {

// LEARNING MAP
// A grid is just a rectangle of cells. A blocked cell cannot be walked through.
// Start with FindPath() in pathfinding.cpp, then study the demo in EditorApp.
struct PathGrid {
  int width = 0;
  int height = 0;
  std::vector<bool> blocked;

  bool IsInside(int x, int y) const;
  bool IsBlocked(int x, int y) const;
};

struct PathSearchFrame {
  std::vector<glm::ivec2> visited;
  std::vector<glm::ivec2> frontier;
  std::vector<glm::ivec2> path;
  bool finished = false;
};

class AStarPathfinder {
public:
  // Returns cells from start to goal. An empty result means no route exists.
  static std::vector<glm::ivec2> FindPath(const PathGrid& grid,
                                          glm::ivec2 start,
                                          glm::ivec2 goal);
  static std::vector<PathSearchFrame> FindPathTrace(const PathGrid& grid,
                                                    glm::ivec2 start,
                                                    glm::ivec2 goal);
};

} // namespace eclipse::systems
