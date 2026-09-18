#pragma once

#include "../ecs/world.h"
#include "assetmanager.h"

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
  bool Load(const std::vector<std::string>& layerAssetIds,
            ecs::World& world, AssetManager& assets,
            const std::shared_ptr<graphics::Mesh>& mesh,
            const std::shared_ptr<graphics::Shader>& shader);
};

} // namespace eclipse::managers
