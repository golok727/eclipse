#pragma once

#include "../graphics/shader.h"
#include "../graphics/texture.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace eclipse::managers {

class AssetManager {
public:
  std::shared_ptr<graphics::Texture> LoadTexture(const std::string& path);

  std::shared_ptr<graphics::Shader> LoadShader(
      const std::string& id,
      const std::string& vertexSource,
      const std::string& fragmentSource);

  std::shared_ptr<graphics::Shader> LoadShaderFromFiles(
      const std::string& id,
      const std::string& vertexPath,
      const std::string& fragmentPath);

  void Clear();

private:
  std::unordered_map<std::string, std::shared_ptr<graphics::Texture>> mTextures;
  std::unordered_map<std::string, std::shared_ptr<graphics::Shader>> mShaders;
};

} // namespace eclipse::managers
