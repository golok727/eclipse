#include "npc_system.h"

#include "../components/gameplaycomponents.h"
#include "../components/rendercomponents.h"

#include <cmath>

namespace eclipse::systems {

void NpcSystem::Update(ecs::World& world, float deltaTime) {
  world.ForEach<components::Transform, components::NpcBehavior>(
      [deltaTime](ecs::Entity, components::Transform& transform,
                  components::NpcBehavior& behavior) {
        const auto target = behavior.movingToEnd ? behavior.patrolEnd
                                                 : behavior.patrolStart;
        const float distance = std::abs(target.x - transform.position.x);
        const float step = behavior.speed * deltaTime;
        const float direction = target.x >= transform.position.x ? 1.0f : -1.0f;
        transform.position.x += direction * step;
        if (distance <= step) {
          transform.position.x = target.x;
          behavior.movingToEnd = !behavior.movingToEnd;
        }
      });
}

} // namespace eclipse::systems
