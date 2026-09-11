#pragma once

#include "../ecs/world.h"

namespace eclipse::systems {

class MovementSystem {
public:
  void Update(ecs::World& world, float deltaTime);
};

} // namespace eclipse::systems
