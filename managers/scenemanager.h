#pragma once

#include "assetmanager.h"
#include "../ecs/world.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace eclipse::managers {

class SceneManager {
public:
  using SceneLoader = std::function<void(ecs::World&, AssetManager&)>;

  void Register(const std::string& name, SceneLoader loader);
  bool Load(const std::string& name, ecs::World& world, AssetManager& assets);
  const std::string& Current() const { return mCurrent; }

private:
  std::unordered_map<std::string, SceneLoader> mLoaders;
  std::string mCurrent;
};

} // namespace eclipse::managers
