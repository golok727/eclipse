#pragma once

#include "../ecs/world.h"

#include <glm/glm.hpp>

namespace eclipse::systems {

class CameraSystem {
public:
  void Update(ecs::World& world, const glm::ivec2& viewport);
};

} // namespace eclipse::systems
