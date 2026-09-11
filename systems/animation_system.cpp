#include "animation_system.h"

#include "../components/gameplaycomponents.h"
#include "../components/rendercomponents.h"

namespace eclipse::systems {

void AnimationSystem::Update(ecs::World& world, float deltaTime) {
  world.ForEach<components::Animation, components::SpriteRenderer>(
      [deltaTime](ecs::Entity, components::Animation& animation,
                  components::SpriteRenderer& sprite) {
        if (animation.frameCount <= 0 || animation.columns <= 0 ||
            animation.rows <= 0 || animation.frameDuration <= 0.0f) {
          return;
        }

        animation.elapsed += deltaTime;
        while (animation.elapsed >= animation.frameDuration) {
          animation.elapsed -= animation.frameDuration;
          animation.currentFrame =
              (animation.currentFrame + 1) % animation.frameCount;
        }

        const int column = animation.currentFrame % animation.columns;
        const int row = animation.currentFrame / animation.columns;
        sprite.uvScale = {1.0f / animation.columns,
                          1.0f / animation.rows};
        sprite.uvOffset = {column * sprite.uvScale.x,
                           row * sprite.uvScale.y};
      });
}

} // namespace eclipse::systems
