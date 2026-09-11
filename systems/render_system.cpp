#include "render_system.h"

// LEARNING MAP
// Rendering happens in two stages: collect visible sprite data, then submit
// draw commands. Follow Render() to see how Transform becomes a model matrix,
// how layer controls draw order, and how RenderMeshTextured finally draws it.

#include "../components/rendercomponents.h"
#include "../graphics/rendercommands.h"
#include "../glm/gtc/matrix_transform.hpp"

#include <algorithm>
#include <vector>

namespace eclipse::systems {

void RenderSystem::Render(ecs::World& world,
                          managers::RenderManager& renderManager) {
  auto* camera = world.GetFirst<components::Camera>();
  if (!camera) {
    return;
  }

  struct RenderItem {
    int layer;
    std::shared_ptr<graphics::Mesh> mesh;
    std::shared_ptr<graphics::Shader> shader;
    std::shared_ptr<graphics::Texture> texture;
    glm::mat4 model;
    glm::vec2 uvOffset;
    glm::vec2 uvScale;
  };
  std::vector<RenderItem> items;

  world.ForEach<components::Transform, components::SpriteRenderer>(
      [&items](ecs::Entity, components::Transform& transform,
               components::SpriteRenderer& sprite) {
        if (!sprite.mesh || !sprite.shader || !sprite.texture) {
          return;
        }

        glm::mat4 model(1.0f);
        model = glm::translate(
            model, glm::vec3(transform.position.x, transform.position.y, 0.0f));
        model = glm::rotate(model, transform.rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(
            model, glm::vec3(transform.scale.x, transform.scale.y, 1.0f));

        items.push_back({sprite.layer, sprite.mesh, sprite.shader, sprite.texture,
                         model, sprite.uvOffset, sprite.uvScale});
      });

  std::stable_sort(items.begin(), items.end(),
                   [](const RenderItem& left, const RenderItem& right) {
                     return left.layer < right.layer;
                   });

  for (const auto& item : items) {
    renderManager.Submit(
        std::make_unique<graphics::rendercommands::RenderMeshTextured>(
            item.mesh, item.shader, item.texture, item.model, camera->view,
            camera->projection, item.uvOffset, item.uvScale));
  }
}

} // namespace eclipse::systems
