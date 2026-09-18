#include "scenemanager.h"

#include "../src/log.h"

#include <utility>

namespace eclipse::managers {

void SceneManager::Register(const std::string& name, SceneLoader loader) {
  mLoaders[name] = std::move(loader);
}

bool SceneManager::Load(const std::string& name, ecs::World& world,
                        AssetManager& assets) {
  const auto it = mLoaders.find(name);
  if (it == mLoaders.end() || !it->second) {
    ECLIPSE_ERROR("Unable to load unknown scene '{}'", name);
    return false;
  }

  world.Clear();
  if (!mCurrent.empty()) {
    assets.UnloadGroup(mCurrent);
  }
  it->second(world, assets);
  mCurrent = name;
  ECLIPSE_INFO("Loaded scene '{}'", name);
  return true;
}

} // namespace eclipse::managers
