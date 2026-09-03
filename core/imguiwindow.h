#pragma once

#include <SDL_events.h>

typedef union SDL_Event SDL_Event;

namespace eclipse::core{
  struct ImGuiWindowProperties{
    bool MoveFromTitleBarOnly = true;
    bool IsViewPortEnabled = true;
    bool IsDockingEnabled = true;
  };
  class ImGuiWindow{
    public:
      ImGuiWindow(){}
      ~ImGuiWindow(){}
      void Create(ImGuiWindowProperties& ImGuiProps);
      void Shutdown();
      void HandleSDLEvents(SDL_Event& event);
      void BeginRender();
      void EndRender();
      bool WantCaptureMouse();
      bool WantCaptureKeyboard();
    private:
  };
}
