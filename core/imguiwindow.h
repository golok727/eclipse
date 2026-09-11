#pragma once
#include <SDL2/SDL.h>
typedef union SDL_Event SDL_Event;

namespace eclipse::core{
  struct ImGuiWindowProperties{
    bool MoveFromTitleBarOnly = false;
    bool IsViewportEnabled = false;
    bool IsDockingEnabled = false;
  };
  class ImGuiWindow{
    public:
      ImGuiWindow(){}
      ~ImGuiWindow(){}
      void Create(const ImGuiWindowProperties& ImGuiProps);
      void Shutdown();
      void HandleSDLEvents(SDL_Event& event);
      void BeginRender();
      void EndRender();
      bool WantToCaptureKeyboard();
      bool WantToCaptureMouse();
    private:
  };
}
