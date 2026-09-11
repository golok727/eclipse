#pragma once

#include "../ecs/world.h"
#include "assetmanager.h"

#include <memory>
#include <string>

namespace eclipse::graphics {
class Mesh;
class Shader;
}

namespace eclipse::managers {

class TiledMapLoader {
public:
  bool Load(const std::string& mapPath,
            ecs::World& world,
            AssetManager& assets,
            const std::shared_ptr<graphics::Mesh>& mesh,
            const std::shared_ptr<graphics::Shader>& shader);
};

} // namespace eclipse::managers
