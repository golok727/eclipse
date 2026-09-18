#pragma once

#include <SDL2/SDL.h>

typedef union SDL_Event SDL_Event;

namespace eclipse::core {

struct ImGuiWindowProperties {
  bool MoveFromTitleBarOnly = false;
  bool IsViewportEnabled = false;
  bool IsDockingEnabled = false;
};

class ImGuiWindow {
public:
  bool Create(const ImGuiWindowProperties& properties);
  void Shutdown();
  void HandleSDLEvents(SDL_Event& event);
  void BeginRender();
  void EndRender();
  bool WantToCaptureKeyboard() const;
  bool WantToCaptureMouse() const;
  bool IsCreated() const { return mCreated; }

private:
  bool mCreated = false;
};

} // namespace eclipse::core
