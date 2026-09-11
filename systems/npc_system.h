#pragma once

#include "../ecs/world.h"

namespace eclipse::systems {

class NpcSystem {
public:
  void Update(ecs::World& world, float deltaTime);
};

} // namespace eclipse::systems
