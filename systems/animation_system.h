#pragma once

#include "../ecs/world.h"

namespace eclipse::systems {

class AnimationSystem {
public:
  void Update(ecs::World& world, float deltaTime);
};

} // namespace eclipse::systems
