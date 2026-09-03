#pragma once
#include "../core/window.h"
namespace eclipse{
  class App{
    public:
      
      App(){}
      ~App(){}
      inline virtual  core::WindowProperties GetWindowProperties(){return core::WindowProperties();}
      virtual void Initialize(){}
      virtual void Shutdown(){}
      virtual void Update(){}
      virtual void Render(){}
      virtual void ImGuiRender(){}
  };
}
