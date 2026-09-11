#include "collision_system.h"

#include "../components/gameplaycomponents.h"
#include "../components/rendercomponents.h"

#include <cmath>
#include <vector>

namespace eclipse::systems {

namespace {

struct Body {
  components::Transform* transform;
  components::Collider* collider;
};

bool Overlaps(const Body& left, const Body& right) {
  const auto leftCenter = left.transform->position + left.collider->offset;
  const auto rightCenter = right.transform->position + right.collider->offset;
  const auto leftHalf = left.collider->size * 0.5f;
  const auto rightHalf = right.collider->size * 0.5f;

  return std::abs(leftCenter.x - rightCenter.x) < leftHalf.x + rightHalf.x &&
         std::abs(leftCenter.y - rightCenter.y) < leftHalf.y + rightHalf.y;
}

} // namespace

void CollisionSystem::Update(ecs::World& world) {
  std::vector<Body> dynamicBodies;
  std::vector<Body> staticBodies;

  world.ForEach<components::Transform, components::Collider>(
      [&dynamicBodies, &staticBodies](ecs::Entity,
                                      components::Transform& transform,
                                      components::Collider& collider) {
        Body body{&transform, &collider};
        (collider.isStatic ? staticBodies : dynamicBodies).push_back(body);
      });

  for (const auto& dynamic : dynamicBodies) {
    for (const auto& solid : staticBodies) {
      if (!Overlaps(dynamic, solid)) {
        continue;
      }

      const auto dynamicCenter =
          dynamic.transform->position + dynamic.collider->offset;
      const auto solidCenter = solid.transform->position + solid.collider->offset;
      const auto dynamicHalf = dynamic.collider->size * 0.5f;
      const auto solidHalf = solid.collider->size * 0.5f;
      const float penetrationX = dynamicHalf.x + solidHalf.x -
                                 std::abs(dynamicCenter.x - solidCenter.x);
      const float penetrationY = dynamicHalf.y + solidHalf.y -
                                 std::abs(dynamicCenter.y - solidCenter.y);

      if (penetrationX < penetrationY) {
        dynamic.transform->position.x +=
            (dynamicCenter.x >= solidCenter.x ? 1.0f : -1.0f) * penetrationX;
      } else {
        dynamic.transform->position.y +=
            (dynamicCenter.y >= solidCenter.y ? 1.0f : -1.0f) * penetrationY;
      }
    }
  }
}

} // namespace eclipse::systems
