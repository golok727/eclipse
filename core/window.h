#pragma once
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <memory>
#include <string>
#include "imguiwindow.h"
#include <glm/glm.hpp>
struct SDL_Window;
using SDL_GLContext = void*;
namespace eclipse::graphics{
  class FrameBuffer;
}
namespace eclipse::core{
  struct WindowProperites{
    std::string title;
    int x,y,w,h;
    int flags;
    int wMin,hMin;
    glm::vec3 clearColor;
    ImGuiWindowProperties ImGuiProps;
    WindowProperites();
  };
  class Window{
    public:
      Window();
      ~Window();
      bool Create(const WindowProperites& props);
      void Shutdown();
      void PollEvents();
      void BeginRender();
      void EndRender();
      void SetAttributes();
      inline graphics::FrameBuffer* GetFrameBuffer(){return mFrameBuffer.get();}
      inline SDL_Window* GetSDLWindow(){return mWindow;}
      inline SDL_GLContext GetGLContext(){return mGLContext;}
      glm::ivec2 GetSize();
    private:
      std::shared_ptr<graphics::FrameBuffer>mFrameBuffer;
      ImGuiWindow mImGuiWindow;
      SDL_Window* mWindow;
      SDL_GLContext mGLContext;
  };
}
