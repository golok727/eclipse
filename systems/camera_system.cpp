#include "camera_system.h"

#include "../components/rendercomponents.h"
#include "../glm/gtc/matrix_transform.hpp"

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

        const float width = static_cast<float>(viewport.x) / camera.zoom;
        const float height = static_cast<float>(viewport.y) / camera.zoom;

        camera.projection = glm::ortho(
            -width * 0.5f, width * 0.5f, height * 0.5f, -height * 0.5f,
            -1.0f, 1.0f);
        camera.view = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(-transform.position.x, -transform.position.y, 0.0f));
      });
}

} // namespace eclipse::systems
