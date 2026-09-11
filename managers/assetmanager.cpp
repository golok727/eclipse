#include "assetmanager.h"

#include "../src/log.h"

#include <fstream>
#include <sstream>

namespace {

std::string ReadTextFile(const std::string& path) {
  std::ifstream file(path);
  if (!file) {
    ECLIPSE_ERROR("Unable to open asset '{}'", path);
    return {};
  }

  std::stringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

} // namespace

namespace eclipse::managers {

std::shared_ptr<graphics::Texture> AssetManager::LoadTexture(
    const std::string& path) {
  auto it = mTextures.find(path);
  if (it != mTextures.end()) {
    return it->second;
  }

  auto texture = std::make_shared<graphics::Texture>(path);
  mTextures.emplace(path, texture);
  return texture;
}

std::shared_ptr<graphics::Shader> AssetManager::LoadShader(
    const std::string& id,
    const std::string& vertexSource,
    const std::string& fragmentSource) {
  auto it = mShaders.find(id);
  if (it != mShaders.end()) {
    return it->second;
  }

  auto shader = std::make_shared<graphics::Shader>(vertexSource, fragmentSource);
  mShaders.emplace(id, shader);
  return shader;
}

std::shared_ptr<graphics::Shader> AssetManager::LoadShaderFromFiles(
    const std::string& id,
    const std::string& vertexPath,
    const std::string& fragmentPath) {
  auto vertexSource = ReadTextFile(vertexPath);
  auto fragmentSource = ReadTextFile(fragmentPath);
  if (vertexSource.empty() || fragmentSource.empty()) {
    return nullptr;
  }
  return LoadShader(id, vertexSource, fragmentSource);
}

void AssetManager::Clear() {
  mShaders.clear();
  mTextures.clear();
}

} // namespace eclipse::managers
