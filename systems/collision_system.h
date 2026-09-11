#pragma once

#include "../ecs/world.h"

namespace eclipse::systems {

class CollisionSystem {
public:
  void Update(ecs::World& world);
};

} // namespace eclipse::systems
