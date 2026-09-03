#pragma once
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <string>
#include "glad/glad.h"
#include "imguiwindow.h"
struct SDL_Window;
using SDL_GLContext = void*;
namespace eclipse::core{
  struct WindowProperties{
    std::string title;
    int x,y,w,h;
    int wMin,hMin;
    int flags;
    float ccR,ccG,ccB;
    ImGuiWindowProperties ImGuiProps;
    WindowProperties();
  };
  class Window{
    public:
      Window();
      ~Window();
      bool Create(WindowProperties& props);
      void Shutdown();
      void PollEvents();
      void BeginRender();
      void EndRender();
      void GetSize(int& w ,int& h);

      inline SDL_Window* GetSDLWindow(){return mWindow;}
      inline SDL_GLContext GetGLContext(){return mGLContext;}

    private:
      ImGuiWindow mImGuiWindow;
      SDL_Window* mWindow;
      SDL_GLContext mGLContext;
  };
}
