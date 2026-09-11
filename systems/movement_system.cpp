#include "movement_system.h"

#include "../components/gameplaycomponents.h"
#include "../components/rendercomponents.h"
#include "../input/keyboard.h"

namespace eclipse::systems {

void MovementSystem::Update(ecs::World& world, float deltaTime) {
  world.ForEach<components::Transform, components::PlayerController>(
      [deltaTime](ecs::Entity, components::Transform& transform,
                  components::PlayerController& controller) {
        transform.previousPosition = transform.position;
        glm::vec2 direction{0.0f};

        if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_LEFT)) {
          direction.x -= 1.0f;
        }
        if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_RIGHT)) {
          direction.x += 1.0f;
        }
        if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_UP)) {
          direction.y += 1.0f;
        }
        if (input::keyboard::Key(input::ECLIPSE_INPUT_KEY_DOWN)) {
          direction.y -= 1.0f;
        }

        transform.position += direction * controller.speed * deltaTime;
      });
}

} // namespace eclipse::systems
