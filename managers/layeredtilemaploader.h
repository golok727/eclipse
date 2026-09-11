#pragma once

#include "assetmanager.h"
#include "../ecs/world.h"

#include <memory>
#include <string>
#include <vector>

namespace eclipse::graphics {
class Mesh;
class Shader;
}

namespace eclipse::managers {

class LayeredTilemapLoader {
public:
  bool Load(const std::string& directory,
            const std::vector<std::string>& layerNames,
            ecs::World& world,
            AssetManager& assets,
            const std::shared_ptr<graphics::Mesh>& mesh,
            const std::shared_ptr<graphics::Shader>& shader);
};

} // namespace eclipse::managers
