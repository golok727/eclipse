#pragma once
#include "framebuffer.h"
#include <memory>
namespace eclipse::graphics {
class Mesh;
class Shader;

namespace rendercommands {
class RenderCommands {
public:
  virtual void Execute() = 0;
  virtual ~RenderCommands() {}
};
class RenderMesh : public RenderCommands {

public:
  virtual void Execute();
  RenderMesh(std::shared_ptr<graphics::Mesh> mesh,
             std::shared_ptr<graphics::Shader> shader)
      : mMesh(mesh), mShader(shader) {}

private:
  std::weak_ptr<graphics::Mesh> mMesh;
  std::weak_ptr<graphics::Shader> mShader;
};



} // namespace rendercommands
} // namespace eclipse::graphics
