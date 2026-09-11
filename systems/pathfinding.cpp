#include "pathfinding.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <queue>

namespace eclipse::systems {

bool PathGrid::IsInside(int x, int y) const {
  return x >= 0 && y >= 0 && x < width && y < height;
}

bool PathGrid::IsBlocked(int x, int y) const {
  if (!IsInside(x, y)) {
    return true;
  }
  return blocked[static_cast<std::size_t>(y * width + x)];
}

namespace {

struct QueueNode {
  int index;
  float score;
};

struct LowestScoreFirst {
  bool operator()(const QueueNode& left, const QueueNode& right) const {
    return left.score > right.score;
  }
};

int Index(const PathGrid& grid, int x, int y) { return y * grid.width + x; }

float Distance(glm::ivec2 left, glm::ivec2 right) {
  return static_cast<float>(std::abs(left.x - right.x) +
                            std::abs(left.y - right.y));
}

} // namespace

std::vector<glm::ivec2> AStarPathfinder::FindPath(const PathGrid& grid,
                                                   glm::ivec2 start,
                                                   glm::ivec2 goal) {
  const auto frames = FindPathTrace(grid, start, goal);
  return frames.empty() ? std::vector<glm::ivec2>{} : frames.back().path;
}

std::vector<PathSearchFrame> AStarPathfinder::FindPathTrace(
    const PathGrid& grid, glm::ivec2 start, glm::ivec2 goal) {
  std::vector<PathSearchFrame> frames;
  if (!grid.IsInside(start.x, start.y) || !grid.IsInside(goal.x, goal.y) ||
      grid.IsBlocked(start.x, start.y) || grid.IsBlocked(goal.x, goal.y)) {
    return frames;
  }

  const int cellCount = grid.width * grid.height;
  const float infinity = std::numeric_limits<float>::infinity();
  std::vector<float> cost(cellCount, infinity);
  std::vector<int> parent(cellCount, -1);
  std::vector<bool> visited(cellCount, false);
  std::priority_queue<QueueNode, std::vector<QueueNode>, LowestScoreFirst> open;

  const int startIndex = Index(grid, start.x, start.y);
  const int goalIndex = Index(grid, goal.x, goal.y);
  cost[startIndex] = 0.0f;
  open.push({startIndex, Distance(start, goal)});

  const glm::ivec2 directions[] = {
      {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  while (!open.empty()) {
    const int currentIndex = open.top().index;
    open.pop();
    if (visited[currentIndex]) {
      continue;
    }
    visited[currentIndex] = true;
    if (currentIndex == goalIndex) {
      break;
    }

    const glm::ivec2 current{currentIndex % grid.width,
                             currentIndex / grid.width};
    for (const auto direction : directions) {
      const glm::ivec2 next = current + direction;
      if (grid.IsBlocked(next.x, next.y)) {
        continue;
      }

      const int nextIndex = Index(grid, next.x, next.y);
      const float newCost = cost[currentIndex] + 1.0f;
      if (newCost >= cost[nextIndex]) {
        continue;
      }

      cost[nextIndex] = newCost;
      parent[nextIndex] = currentIndex;
      open.push({nextIndex, newCost + Distance(next, goal)});
    }

    PathSearchFrame frame;
    for (int index = 0; index < cellCount; ++index) {
      if (visited[index]) {
        frame.visited.push_back({index % grid.width, index / grid.width});
      }
    }
    auto pending = open;
    while (!pending.empty()) {
      const int index = pending.top().index;
      pending.pop();
      if (!visited[index]) {
        frame.frontier.push_back({index % grid.width, index / grid.width});
      }
    }
    frames.push_back(std::move(frame));
  }

  if (parent[goalIndex] == -1 && goalIndex != startIndex) {
    if (!frames.empty()) {
      frames.back().finished = true;
    }
    return frames;
  }

  std::vector<glm::ivec2> path;
  for (int index = goalIndex; index != -1; index = parent[index]) {
    path.push_back({index % grid.width, index / grid.width});
  }
  std::reverse(path.begin(), path.end());
  PathSearchFrame finalFrame;
  if (!frames.empty()) {
    finalFrame = frames.back();
  }
  finalFrame.path = std::move(path);
  finalFrame.finished = true;
  frames.push_back(std::move(finalFrame));
  return frames;
}

} // namespace eclipse::systems
