#include "layeredtilemaploader.h"

#include "../components/rendercomponents.h"
#include "../src/log.h"

namespace eclipse::managers {

bool LayeredTilemapLoader::Load(
    const std::vector<std::string>& layerAssetIds, ecs::World& world,
    AssetManager& assets, const std::shared_ptr<graphics::Mesh>& mesh,
    const std::shared_ptr<graphics::Shader>& shader) {
  if (layerAssetIds.empty() || !mesh || !shader) {
    ECLIPSE_ERROR("Unable to load layered tilemap: missing layers or renderer");
    return false;
  }

  int loadedLayers = 0;
  for (std::size_t index = 0; index < layerAssetIds.size(); ++index) {
    const auto texture = assets.GetTexture(layerAssetIds[index]);
    if (!texture || !texture->IsLoadedFromSource()) {
      ECLIPSE_ERROR("Unable to load tilemap layer asset '{}'",
                    layerAssetIds[index]);
      continue;
    }

    const auto entity = world.CreateEntity();
    world.Add<components::Transform>(
        entity,
        components::Transform{
            {texture->GetWidth() * 0.5f, texture->GetHeight() * 0.5f},
            {0.0f, 0.0f},
            0.0f,
            {static_cast<float>(texture->GetWidth()),
             -static_cast<float>(texture->GetHeight())}});
    auto& sprite = world.Add<components::SpriteRenderer>(
        entity, components::SpriteRenderer{mesh, shader, texture});
    sprite.layer = static_cast<int>(index);
    ++loadedLayers;
  }

  ECLIPSE_INFO("Loaded {} of {} layered tilemap assets", loadedLayers,
               layerAssetIds.size());
  return loadedLayers == static_cast<int>(layerAssetIds.size());
}

} // namespace eclipse::managers
