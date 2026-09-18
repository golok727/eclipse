#pragma once

#include <glm/glm.hpp>

namespace eclipse::components {

struct PlayerController {
  float speed = 250.0f;
};

struct Collider {
  glm::vec2 size{1.0f};
  glm::vec2 offset{0.0f};
  bool isStatic = false;
};

struct Animation {
  int frameCount = 1;
  int columns = 1;
  int rows = 1;
  float frameDuration = 0.1f;
  float elapsed = 0.0f;
  int currentFrame = 0;
};

struct NpcBehavior {
  glm::vec2 patrolStart{0.0f};
  glm::vec2 patrolEnd{0.0f};
  float speed = 20.0f;
  bool movingToEnd = true;
};

} // namespace eclipse::components
