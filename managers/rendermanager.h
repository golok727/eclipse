#pragma once
#include "../graphics/rendercommands.h"
#include <memory>
#include <queue>
#include <stack>
#include "../graphics/framebuffer.h"
// #define ECLIPSE_SUBMIT_RC(type,...)std::move(std::make_unique<eclipse::graphics::rendercommands::type>(__VA_ARGS__))



namespace eclipse::managers{
  class RenderManager{
    public:
      RenderManager(){}
      ~RenderManager(){}
      void Initialize();
      void Shutdown();
      void SetClearColor(float r , float g , float b , float a);
      void Clear();
      void Submit(std::unique_ptr<graphics::rendercommands::RenderCommands>rc);
      void Flush();
      void SetWireFrameMode(bool enabled);
      
    private:

      std::queue<std::unique_ptr<graphics::rendercommands::RenderCommands>>mRenderCommands;      

  };
}
