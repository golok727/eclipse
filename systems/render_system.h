#pragma once

#include "../ecs/world.h"
#include "../managers/rendermanager.h"

namespace eclipse::systems {

class RenderSystem {
public:
  void Render(ecs::World& world, managers::RenderManager& renderManager);
};

} // namespace eclipse::systems
