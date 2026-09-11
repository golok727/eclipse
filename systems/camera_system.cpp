#include "camera_system.h"

#include "../components/rendercomponents.h"
#include "../glm/gtc/matrix_transform.hpp"

#include <algorithm>

namespace eclipse::systems {

void CameraSystem::Update(ecs::World& world, const glm::ivec2& viewport) {
  world.ForEach<components::Transform, components::Camera>(
      [&world, &viewport](ecs::Entity, components::Transform& transform,
                  components::Camera& camera) {
        if (camera.followTarget != ecs::NullEntity &&
            world.IsAlive(camera.followTarget)) {
          auto* target = world.Get<components::Transform>(camera.followTarget);
          if (target) {
            transform.position = target->position;
          }
        }

        float zoom = camera.zoom;
        if (camera.fitSize.x > 0.0f && camera.fitSize.y > 0.0f) {
          zoom = std::min(static_cast<float>(viewport.x) / camera.fitSize.x,
                          static_cast<float>(viewport.y) / camera.fitSize.y) *
                 camera.fitScale;
        }

        const float width = static_cast<float>(viewport.x) / zoom;
        const float height = static_cast<float>(viewport.y) / zoom;

        camera.projection = glm::ortho(
            -width * 0.5f, width * 0.5f, height * 0.5f, -height * 0.5f,
            -1.0f, 1.0f);
        camera.view = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(-transform.position.x, -transform.position.y, 0.0f));
      });
}

} // namespace eclipse::systems
